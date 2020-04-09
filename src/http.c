/*
 * Source file: http.c
 * 
 * Connects to and accesses web servers via HTTP GET.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "http.h"
#include "url.h"
#include "client.h"

// Search strings
#define CONTENT_LENGTH  "content-length: "
#define LOCATION        "location: "
#define CONTENT_TYPE    "content-type: "
#define HEADER_BOUNDARY "\r\n\r\n"
#define END_OF_LINE     "\r\n"

// Memory allocations
#define MAX_HEADER_LENGTH   4096
#define CHUNK_HEADER_LENGTH 10

// Flags
#define CHUNKED          -1
#define INVALID_RESPONSE -6

/*
 * Function: strlower
 * 
 * Copies bytes from src to dest as lowercase ASCII.
 * 
 * char *dest: Destination address.
 * char *src:  Source address.
 */
void strlower(char *dest, char *src) {
	for (int i = 0; i < (int)strlen(src); i++) {
		dest[i] = tolower(src[i]);
	}
}

/*
 * Function: get_value
 * 
 * Finds where in the response header a field's value is located,
 * maintaining any value case sensitivity.
 * 
 * char *response:    Server response header.
 * const char *field: Field name to be searched for.
 * 
 * Returns char*: Pointer to location within response or NULL if not found.
 */
char *get_value(char *response, const char *field) {

	char l_response[MAX_RESPONSE_LENGTH] = {0};
	strlower(l_response, response);

	// Count how many bytes into the lowercase response that the field was found
	return (response + (strstr(l_response, field) - l_response) + strlen(field));
}

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

	char *content_length = get_value(response, CONTENT_LENGTH);

	return content_length ? atol(content_length) : CHUNKED;
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

	char *location = get_value(response, LOCATION);

	// Reset URL and use the new one
	memset(url, 0, MAX_URL_LENGTH);
	memmove(url, location, strstr(location, END_OF_LINE) - location);
}

/*
 * Function: get_content_type
 * 
 * Checks the MIME-type of the content to be received.
 * 
 * char *response: Server response header.
 * char *accept:   MIME types the client can accept.
 * 
 * Returns int: 1 if content type is as passed, 0 otherwise.
 */
int is_content_type(char *response, char *accept) {

	char *content_type = get_value(response, CONTENT_TYPE);

	return content_type ? strstr(content_type, accept) ? 1 : 0 : 0;
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
long get_chunk_length(int sockfd) {
	
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
long get_chunked_response(int sockfd, char *response, long *expected_length) {

	long next_read, bytes_read = 0;
	*expected_length = 0;

	next_read = get_chunk_length(sockfd);
	*expected_length += next_read;

	while (next_read) {

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
 * char *flag:     Flag for response handling (flags are defined in http.h).
 * 
 * Returns int: HTTP status code or error code.
 */
int http_get(char *url, char *response, char *flag) {

	// Setup memory
	char header[MAX_HEADER_LENGTH] = {0}, host[MAX_URL_LENGTH] = {0}, path[MAX_URL_LENGTH] = {0};
	int sockfd, status;
	long bytes_read, expected_length = 0;

	memset(response, 0, MAX_RESPONSE_LENGTH);
	
	// Parse url string
	get_host(url, host);
	get_path(url, path);

	// Generate request header
	int n;
	n = sprintf(header, "GET %s %s%s", path, HTTP_VER, END_OF_LINE);
	n += sprintf(header + n, "Host: %s%s", host, END_OF_LINE);
	n += sprintf(header + n, "Connection: %s%s", CONNECTION, END_OF_LINE);
	n += sprintf(header + n, "User-Agent: %s%s", USER_AGENT, END_OF_LINE);
	n += sprintf(header + n, "Accept: %s%s", ACCEPT, END_OF_LINE);
	if (*flag == AUTH_REQUIRED) {
		n += sprintf(header + n, "Authorization: %s%s", AUTH, END_OF_LINE);
	}
	sprintf(header + n, "%s%s", END_OF_LINE, END_OF_LINE);

	// Establish connection and send HTTP request to server
	sockfd = establish(host, PORT, header);

	if (sockfd < 0) {
		return sockfd;
	}

	// Get response header
	bytes_read = read_response(sockfd, response, HEADER_BOUNDARY, HEADER_MODE);

	if (bytes_read < 0) {
		close_socket(sockfd);
		return (int)bytes_read;
	}

	// HTTP status code
	status = get_status(response);

	// Pre-handling of status code
	switch (status) {

		/* All good. */
		case 200:
			break;
		
		/* New URL for this resource. Get new URL
		   and re-attempt get. */
		case 301:
			*flag = REDIRECTED;

			get_location(url, response);
			close_socket(sockfd);

			return http_get(url, response, flag);
		
		/* Authorisation required. Reattempt with
		   authorisation header. */
		case 401:
			*flag = AUTH_REQUIRED;
			return http_get(url, response, flag);
			break;
		
		/* Page not found, do not re-attempt
		   but do return the response. */
		case 404:
			*flag = PERMANENT_FAILURE;
			break;
		
		/* Page gone, do not re-attempt
		   but do return the response. */
		case 410:
			*flag = PERMANENT_FAILURE;
			break;
		
		/* URI too long, do not re-attempt
		   but do return the response. */
		case 414:
			*flag = PERMANENT_FAILURE;
			break;
		
		/* Service unavailable, possibly overloaded
		   or some other temporary failure. Reattempt
			 get. */
		case 503:
			close_socket(sockfd);

			return http_get(url, response, flag);
		
		/* Gateway timeout, possibly overloaded
		   or some other temporary failure. Reattempt
			 get. */
		case 504:
			close_socket(sockfd);

			return http_get(url, response, flag);
	}

	// Only accept the MIME-type defined by ACCEPT
	if (!is_content_type(response, ACCEPT)) {

		*flag = CONTENT_TYPE_NA;
		return status;
	}

	expected_length = get_content_length(response);

	// Reset buffer
	memset(response, 0, MAX_RESPONSE_LENGTH);

	// Read content
	bytes_read = expected_length == CHUNKED ?
							 get_chunked_response(sockfd, response, &expected_length) :
							 read_response(sockfd, response, NULL, expected_length);

	close_socket(sockfd);

	if (expected_length != bytes_read) {
		if (*flag == TRUNCATED) {

			// Twice truncated response, do not reattempt
			return status;
		} else {

			// Truncated response, reattempt get
			return http_get(url, response, flag);
		}
	}

	// Overrite TRUNCATED flag
	*flag = (*flag == TRUNCATED) || !(*flag) ? OK : *flag;

	return status;
}
