#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"
#include "client.h"

#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP_VER   "HTTP/1.1"
#define PORT       80

#define CONTENT_LENGTH  "Content-Length: "
#define HEADER_BOUNDARY "\r\n\r\n"
#define CHUNK_BOUNDARY  "\r\n"

#define MAX_URL_LENGTH      1000
#define MAX_HEADER_LENGTH   256
#define MAX_RESPONSE_LENGTH 100000

#define CHUNKED -1

#define INVALID_URL      -5
#define INVALID_RESPONSE -6

/*
 * Function: parse_header
 * 
 * Handles the HTTP response header from the server.
 * 
 * char *response:       Server response.
 * long *content_length: What the server says the response length will be.
 * 
 * Returns int: HTTP status code.
 */
int parse_header(char *response, long *content_length) {

	// Extract content length or handle as chunked transfer
	*content_length = strstr(response, CONTENT_LENGTH) == NULL ?
										CHUNKED :
										atol(strstr(response, CONTENT_LENGTH) + strlen(CONTENT_LENGTH));

	// Extract status code
	return atoi(strchr(response, ' ') + 1);
}

/*
 * Function: get_chunk_length
 * 
 * Reads the chunk header to get next chunk length. Chunk headers are expressed
 * as a hex string terminated by \r\n.
 * 
 * int *sockfd: Socket file descriptor.
 * 
 * Returns long: Length of chunk in bytes.
 */
long get_chunk_length(int *sockfd) {

	char buffer[BUFFER_LENGTH];

	read_response(sockfd, buffer, CHUNK_BOUNDARY, 1);

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

	long next_read;
	*expected_length = 0;

	// Get chunk length
	next_read = get_chunk_length(sockfd);
	*expected_length += next_read;

	long bytes_read = 0;
	while (next_read) {

		bytes_read += read_response(sockfd, response + bytes_read, CHUNK_BOUNDARY, next_read + strlen(CHUNK_BOUNDARY));

		// Ignore chunk boundary bytes from content length
		bytes_read -= strlen(CHUNK_BOUNDARY);

		// Get chunk length
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
	response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));
	URL *_url = (URL*)malloc(MAX_URL_LENGTH * sizeof(char));
	char *header = (char*)malloc(MAX_HEADER_LENGTH * sizeof(char));
	int *sockfd, status;
	long bytes_read, expected_length;

	memset(response, 0, MAX_RESPONSE_LENGTH);
	memset(_url, 0, MAX_URL_LENGTH);
	memset(header, 0, MAX_HEADER_LENGTH);
	
	// Parse url string
	if (parse_url(url, _url) < 0) {
		
		free(_url);
		return INVALID_URL;
	}

	// Generate request header
	snprintf(
		header,
		MAX_HEADER_LENGTH,
		"GET %s %s\r\nHost: %s\r\nConnection: %s\r\nUser-Agent: %s\r\nAccept: %s\r\n\r\n",
		_url->path, HTTP_VER, _url->host, CONNECTION, USER_AGENT, ACCEPT
	);

	// Send HTTP request to server
	sockfd = establish(_url->host, PORT, header);
	free(_url);
	free(header);

	// Get response header
	bytes_read = read_response(sockfd, response, HEADER_BOUNDARY, 1);

	if (bytes_read < 0) {
		return (int)bytes_read;
	}

	// Get response data
	status = parse_header(response, &expected_length);

	if (status < 0) {
		return status;
	}

	// Reset response data
	memset(response, 0, MAX_RESPONSE_LENGTH);

	// Read content from server
	bytes_read = expected_length == CHUNKED ?
								get_chunked_response(sockfd, response, &expected_length) :
								read_response(sockfd, response, NULL, expected_length);

	close_socket(sockfd);

	return status;
}
