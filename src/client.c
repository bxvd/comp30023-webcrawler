#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include "url.h"
#define BUFFER_SIZE 256

#define SERVER_NOT_FOUND -1
#define SOCKET_ERROR     -2
#define CONNECTION_ERROR -3

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
 */
int request(char *host, int port, char *header, char *response) {

	// Get IP address from host name
	struct hostent *addr = gethostbyname(host);

	if (addr == NULL) {
		return SERVER_NOT_FOUND;
	}

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	int sockfd = setup_socket(addr, port);

	// Problem setting up socket
	if (sockfd < 0) {
		return sockfd;
	}

	// Send request header
	write(sockfd, header, strlen(header));

	// Read data from server
	int i = 0;
	for (i = 0; read(sockfd, buffer, BUFFER_SIZE - 1); i += (BUFFER_SIZE - 1)) {

		memmove(&response[i], buffer, BUFFER_SIZE);
		memset(buffer, 0, BUFFER_SIZE);
	}

	// Close connection
	close(sockfd);

	return 0;
}
