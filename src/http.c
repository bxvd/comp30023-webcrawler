#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "url.h"
#include "client.h"

// Search strings
#define CONTENT_LENGTH  "Content-Length: "
#define LOCATION        "Location: "
#define CONTENT_TYPE    "Content-Type: "
#define HEADER_BOUNDARY "\r\n\r\n"
#define END_OF_LINE     "\r\n"

// Memory allocations
#define MAX_HEADER_LENGTH   256
#define MAX_RESPONSE_LENGTH 100000
#define CHUNK_HEADER_LENGTH 10

// Flags
#define CHUNKED            -1
#define INVALID_RESPONSE   -6
#define TRUNCATED_RESPONSE -8
#define CONTENT_TYPE_NA    -9

/*
 * Function: get_status
 * 
 * Gets HTTP status code from response header.
 * 
 * char *response: Server response header.
 * 
 * Returns int: Status code.
 */
int get_status(char *response) {
	return atoi(strchr(response, ' ') + 1);
}

/*
 * Function: get_content_length
 * 
 * Gets expected content length from response header.
 * 
 * char *response: Server response header.
 * 
 * Returns long: Chunked encoding flag or expected length of content.
 */
long get_content_length(char *response) {
	return strstr(response, CONTENT_LENGTH) == NULL ?
				   CHUNKED :
				   atol(strstr(response, CONTENT_LENGTH) + strlen(CONTENT_LENGTH));
}

/*
 * Function: get_location
 * 
 * Gets new location from response header for a redirect.
 * 
 * char *url:      Pointer to the url to be updated with the new location.
 * char *response: Server response header.
 */
void get_location(char *url, char *response) {

	char *location = strstr(response, LOCATION) + strlen(LOCATION);

	memset(url, 0, MAX_URL_LENGTH);
	memmove(url, location, strstr(location, END_OF_LINE) - location);

	fprintf(stderr, "Redirect to: %s\n", url);
}

/*
 * Function: get_content_type
 * 
 * Gets the MIME-type of the content to be received.
 * 
 * char *response: Server response header.
 * 
 * Returns char*: Pointer to location in response that specifies the content type.
 */
char *get_content_type(char *response) {

	return strstr(response, CONTENT_TYPE) + strlen(CONTENT_TYPE);
}

/*
 * Function: get_chunk_length
 * 
 * Reads the chunk header to get next chunk length.
 * 
 * int *sockfd: Socket file descriptor.
 * 
 * Returns long: Length of chunk in bytes.
 */
long get_chunk_length(int *sockfd) {
	
	/*
	 * Chunk headers are expressed as a hex string terminated by CRLF.
	 */

	char buffer[CHUNK_HEADER_LENGTH];

	read_response(sockfd, buffer, END_OF_LINE, HEADER_MODE);

	return strtol(buffer, NULL, 16);
}

/*
 * Function: get_chunked_response
 * 
 * Reads content from server in chunked encoding format.
 * 
 * int *sockfd:           Socket file descriptor.
 * char *response:        String to store the server response.
 * long *expected_length: Pointer to store the total expected length of the content.
 * 
 * Returns long: Actual bytes read from the server.
 */
long get_chunked_response(int *sockfd, char *response, long *expected_length) {

	fprintf(stderr, "Begin chunked transfer.\n");

	long next_read, bytes_read = 0;
	*expected_length = 0;

	next_read = get_chunk_length(sockfd);
	*expected_length += next_read;

	while (next_read) {

		fprintf(stderr, "Get chunk size: %ld\n", next_read);

		bytes_read += read_response(sockfd, response + bytes_read, END_OF_LINE, next_read + strlen(END_OF_LINE));

		// Ignore chunk boundary bytes
		bytes_read -= strlen(END_OF_LINE);

		next_read = get_chunk_length(sockfd);
		*expected_length += next_read;
	}

	return bytes_read;
}

/*
 * Function: http_get
 * 
 * Generates a HTTP GET header and captures the response.
 * 
 * char *url:      URL string of server.
 * char *response: Pointer to the location of where to put the response.
 * 
 * Returns int: HTTP status code or error code.
 */
int http_get(char *url, char *response) {

	// Setup memory
	char *header = (char*)malloc(MAX_HEADER_LENGTH * sizeof(char));
	char host[MAX_URL_LENGTH], path[MAX_URL_LENGTH];
	int *sockfd, status;
	long bytes_read, expected_length = 0;

	memset(response, 0, MAX_RESPONSE_LENGTH);
	memset(header, 0, MAX_HEADER_LENGTH);
	
	// Parse url string
	get_host(url, host);
	get_path(url, path);

	// Generate request header
	snprintf(
		header,
		MAX_HEADER_LENGTH,
		"GET %s %s\r\nHost: %s\r\nConnection: %s\r\nUser-Agent: %s\r\nAccept: %s\r\n\r\n",
		path, HTTP_VER, host, CONNECTION, USER_AGENT, ACCEPT
	);

	fprintf(stderr, "Request header:\n%s\n", header);

	// Send HTTP request to server
	sockfd = establish(host, PORT, header);

	if (*sockfd < 0) {
		return *sockfd;
	}

	//fprintf(stderr, "Connected to %s, request header:\n%s\n\n", _url->host, header);
	fprintf(stderr, "Connected to %s. ", host);

	free(header);

	// Get response header
	bytes_read = read_response(sockfd, response, HEADER_BOUNDARY, HEADER_MODE);

	if (bytes_read < 0) {
		return (int)bytes_read;
	}

	// HTTP status code
	status = get_status(response);

	// Only accept the MIME-type defined by ACCEPT
	if (
			strnstr(
				get_content_type(response),
				ACCEPT,
				strstr(get_content_type(response), HEADER_BOUNDARY) - get_content_type(response)
			) == NULL) {
		return CONTENT_TYPE_NA;
	}

	fprintf(stderr, "Response header:\n%s\n", response);
	//fprintf(stderr, "Status: %d\n", status);

	// Handle status codes
	switch (status) {

		/*
		 * Permanent redirect. Equivalent to the original URL being for the same
		 * page as the new URL, so overrite the old URL and continue.
		 */
		case 301:
			get_location(url, response);
			close_socket(sockfd);
			return http_get(url, response);
		
		default:
			break;
	}


	// if (status != 200) {
	// 	return status;
	// }

	expected_length = get_content_length(response);

	// Read content from server
	bytes_read = expected_length == CHUNKED ?
							   get_chunked_response(sockfd, response, &expected_length) :
							   read_response(sockfd, response, NULL, expected_length);
	
	fprintf(stderr, "Expected length: %ld, bytes read: %ld\n", expected_length, bytes_read);

	if (expected_length != bytes_read) {

		fprintf(stderr, "Truncated response.\n");
	}

	close_socket(sockfd);

	return status;
}
