#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "url.h"

#define BUFFER_LENGTH 1024

#define HEADER_MODE 1

#define SERVER_NOT_FOUND  -1
#define SOCKET_ERROR      -2
#define CONNECTION_ERROR  -3
#define SERVER_READ_ERROR -4

/*
 * Function: setup_socket
 * 
 * Performs logic to establish an IP connection via sockets.
 * 
 * int *sockfd:          Socket file descriptor.
 * struct hostent *addr: Parsed destination IP address.
 * int port:             Destination port number.
 */
void setup_socket(int sockfd, struct hostent *addr, int port) {

	struct sockaddr_in server_addr;

	// Prepare address data structure
	memset((char*)&server_addr, 0, sizeof(server_addr));

	// Set address family to IPv4
	server_addr.sin_family = AF_INET;

	// Set server address
	memmove((char*)&server_addr.sin_addr.s_addr, addr->h_addr_list[0], addr->h_length);

	// Set port
	server_addr.sin_port = htons(port);

	*sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		sockfd = SOCKET_ERROR;
		return;
	}

	// Set up connection
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		sockfd = CONNECTION_ERROR;
		return;
	}
}

/*
 * Function: establish
 * 
 * Establishes a connection with a server.
 * 
 * char *host:   Server hostname.
 * int port:     Destination port number.
 * char *header: Request header.
 * 
 * Returns int*: Pointer to a socket file descriptor.
 */
int establish(char *host, int port, char *header) {
	
	// Persistent sockfd to keep socket open
	//int *sockfd = (int*)malloc(sizeof(int));
	int sockfd = 0;

	// Get IP address from host name
	struct hostent *addr = gethostbyname(host);

	if (addr == NULL) {
		sockfd = SERVER_NOT_FOUND;
		return sockfd;
	}

	setup_socket(sockfd, addr, port);

	// Problem setting up socket
	if (sockfd < 0) {
		return sockfd;
	}

	// Send request header
	write(sockfd, header, strlen(header));

	return sockfd;
}

/*
 * Function: read_response
 * 
 * Reads bytes from a server via sockets.
 * 
 * int *sockfd:         Socket file descriptor.
 * char *response:      String to store the bytes read from the server.
 * char *boundary:      Stops reading bytes if it encounters a 'boundary' string until the function is called again.
 * long content_length: Maximum number of bytes to read from the server.
 * 
 * Returns long: Length of data read in bytes.
 */
long read_response(int sockfd, char *response, char *boundary, long content_length) {

	/*
	 * Function is in header mode when content_length = 1.
	 * Reads one byte at a time until 'boundary' is encountered so as to precisely capture data.
	 */

	// Set buffer length based on expected length
	long bytes_read = 0, buffer_length = content_length;

	content_length = content_length == HEADER_MODE ? BUFFER_LENGTH : content_length;

	while (bytes_read < content_length) {

		bytes_read += read(sockfd, response + bytes_read, buffer_length);
		
		// Check if a boundary string has been encountered
		if (boundary) {
			if (bytes_read >= (long)strlen(boundary) && strstr(response, boundary)) {
				return bytes_read;
			}
		}

		// Prevent reading more bytes than expected
		buffer_length = buffer_length == HEADER_MODE ? buffer_length : content_length - bytes_read;
	}

	return bytes_read;
}

/*
 * Function: close_socket
 * 
 * int *sockfd: Socket file descriptor.
 */
void close_socket(int sockfd) {
	close(sockfd);
	//free(sockfd);
}
