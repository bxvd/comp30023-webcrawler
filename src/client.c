#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_NOT_FOUND 1

int open_connection(char *hostname) {

	// Get address
	struct hostent *server = gethostbyname(hostname);

	// Failed connection
	if (server == NULL) {
		return SERVER_NOT_FOUND;
	}

	return 0;
}
