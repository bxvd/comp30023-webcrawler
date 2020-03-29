#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include "url.h"

#define BUFFER_LENGTH 1024

#define HEADER_MODE 1

#define SERVER_NOT_FOUND  -1
#define SOCKET_ERROR      -2
#define CONNECTION_ERROR  -3
#define SERVER_READ_ERROR -4

long min(long a, long b) {
	return a < b ? a : b;
}

long max(long a, long b) {
	return a > b ? a : b;
}

/*
 * Function: setup_socket
 * 
 * Performs logic to establish an IP connection via sockets.
 * 
 * struct hostent *addr: Parsed destination IP address.
 * int port:             Destination port number.
 * 
 * Returns int: Socket file descriptor or error code.
 */
int setup_socket(struct hostent *addr, int port) {

	int sockfd;
	struct sockaddr_in server_addr;

	// Prepare address data structure
	memset((char*)&server_addr, 0, sizeof(server_addr));

	// Set address family to IPv4
	server_addr.sin_family = AF_INET;

	// Set server address
	memmove((char*)&server_addr.sin_addr.s_addr, addr->h_addr_list[0], addr->h_length);

	// Set port
	server_addr.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		return SOCKET_ERROR;
	}

	// Set up connection
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		return CONNECTION_ERROR;
	}

	return sockfd;
}

int establish(char *host, int port, char *header) {

	// Get IP address from host name
	struct hostent *addr = gethostbyname(host);

	if (addr == NULL) {
		return SERVER_NOT_FOUND;
	}

	int sockfd = setup_socket(addr, port);

	// Problem setting up socket
	if (sockfd < 0) {
		return sockfd;
	}

	// Send request header
	write(sockfd, header, strlen(header));

	return sockfd;
}

long read_response(int sockfd, char *response, char *boundary, long content_length) {

	long bytes_read = 0, buffer_length = min(BUFFER_LENGTH, content_length);
	content_length = content_length == HEADER_MODE ? BUFFER_LENGTH : content_length;

	fprintf(stderr, "Reading %ld @ %ld\n", content_length, buffer_length);

	while (bytes_read < content_length) {

		bytes_read += read(sockfd, response + bytes_read, buffer_length);
		
		if ((bytes_read >= (long)strlen(boundary)) && (strstr(response, boundary) != NULL)) {
			fprintf(stderr, "Boundary reached.\n");
			return bytes_read;
		}

		buffer_length = min(buffer_length, content_length - bytes_read);
	}

	fprintf(stderr, "Bytes read %ld\n", bytes_read);

	return bytes_read;
}

void close_socket(int sockfd) {
	close(sockfd);
}

/*
 * Function: request
 * 
 * Performs a request over IP and reads data from the server.
 * 
 * char *host:     Destination hostname.
 * int port:       Destination port number.
 * char *header:   A complete request header.
 * char *response: Pointer to where the server's response is to be copied to.
 * 
 * Returns int Error code or 0 if successful.
 *
int request(char *host, int port, char *header, char *response, size_t length) {

	// Get IP address from host name
	struct hostent *addr = gethostbyname(host);

	if (addr == NULL) {
		return SERVER_NOT_FOUND;
	}

	// char buffer[max];
	// memset(buffer, 0, max);

	int sockfd = setup_socket(addr, port);

	// Problem setting up socket
	if (sockfd < 0) {
		return sockfd;
	}

	// Send request header
	write(sockfd, req_header, strlen(req_header));

	// Read data from server

	char *res_header = get_header(sockfd);

	// size_t bytes_read = 0;
	// while (bytes_read < max_length) {

	// 	long bytes_received = read(sockfd, response + bytes_read, max_length - bytes_read);

	// 	if (bytes_received < 0) {
	// 		return SERVER_READ_ERROR;
	// 	}

	// 	bytes_read += bytes_received;
	// }


	//size_t bytes_read;
	//for (long bytes_read = 0; bytes_read < max; bytes_read += read(sockfd, response + bytes_read, max - bytes_read));

	// for (i = 0; read(sockfd, buffer, BUFFER_SIZE - 1); i += (BUFFER_SIZE - 1)) {

	// 	memmove(&response[i], buffer, BUFFER_SIZE);
	// 	memset(buffer, 0, BUFFER_SIZE);
	// }

	// Close connection
	close(sockfd);

	return 0;
}*/
