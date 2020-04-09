/*
 * Source file: client.c
 * 
 * Provides functions for client-side network communication.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 */

#define HEADER_MODE 1

/*
 * Function: establish
 * 
 * Establishes TCP connection with a server.
 * 
 * char *host:   Server hostname.
 * int port:     Destination port number.
 * char *header: Request header.
 * 
 * Returns int: A socket file descriptor.
 */
int establish(char *host, int port, char *header);

/*
 * Function: read_response
 * 
 * Reads bytes from a server via sockets.
 * 
 * int  sockfd:         Socket file descriptor.
 * char *response:      String to store the bytes read from the server.
 * char *boundary:      Stops reading bytes if it encounters a 'boundary' string until the function is called again.
 * long content_length: Maximum number of bytes to read from the server.
 * 
 * Returns long: Length of data read in bytes.
 */
long read_response(int sockfd, char *response, char *boundary, size_t length);

/*
 * Function: close_socket
 * 
 * Closes TCP connection.
 * 
 * int sockfd: Socket file descriptor.
 */
void close_socket(int sockfd);
