#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "url.h"

#define INVALID_URL -5

/*
 * Function: get_protocol
 * 
 * Takes in a URL string and extracts the protocol.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the protocol.
 */
void get_protocol(char *url, char *protocol) {

	assert(url && protocol);

	char *element_end = strstr(url, PROTOCOL_DELIMITER);
	char *element_start = element_end ? url : DEFAULT_PROTOCOL;
	int element_length = element_end ? (size_t)element_end - (size_t)url : strlen(DEFAULT_PROTOCOL);

	memmove(protocol, element_start, element_length);
}

/*
 * Function: get_host
 * 
 * Takes in a URL string and extracts the hostname.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the hostname.
 */
void get_host(char *url, char *host) {

	assert(url && host);

	char *element_start = strstr(url, PROTOCOL_DELIMITER);

	if (element_start) {
		// URL begins with a protocol
		element_start += strlen(PROTOCOL_DELIMITER);
	} else if ((element_start = strstr(url, LOCATION_DELIMITER))) {
		// URL begins with '//'
		element_start += strlen(LOCATION_DELIMITER);
	} else {
		// URL begins with the hostname
		element_start = url;
	}

	char *element_end = strstr(element_start, HOST_DELIMITER);
	element_end = element_end ? element_end : (url + strlen(url));

	memmove(host, element_start, element_end - element_start);
}

/*
 * Function: get_path
 * 
 * Takes in a URL string and extracts the path.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the path.
 */
void get_path(char *url, char *path) {

	assert(url && path);

	// URL begins with a protocol
	char *element_start = strstr(url, PROTOCOL_DELIMITER);
	element_start += element_start ? strlen(PROTOCOL_DELIMITER) : 0;
	
	if (element_start == NULL) {

		// URL begins with '//'
		element_start = strlen(LOCATION_DELIMITER);
		element_start += element_start ? strlen(LOCATION_DELIMITER) : 0;
	}

	// URL begins with the hostname
	element_start = element_start ? element_start : strstr(url, HOST_DELIMITER);

	// URL doesn't contain a path at all
	element_start = element_start ? element_start : DEFAULT_PATH;

	memmove(path, element_start, strlen(element_start));
}

/*
 * Function: stringify_url
 * 
 * Generates a valid URL string.
 * 
 *            int n:          Number of url arguments passed.
 *            char *path:     Path to resource (or the the URL itself to be stringified).
 * (optional) char *host:     Hostname.
 * (optional) char *protocol: Protocol to be used ('http' if none provided).
 * 
 * returns char*: URL string.
 */
char *stringify_url(int n, char *path, ...) {

	assert(n <= 3);
	assert(path != NULL);

	// Handle variable number of arguments
	va_list ap;
	va_start(ap, path);

	char *host = n >= 2 ? va_arg(ap, char*) : NULL;
	char *protocol = n == 3 ? va_arg(ap, char*) : strstr(host, PROTOCOL_DELIMITER) ? NULL : "http";

	// Set protocol delimiter
	char *pd = protocol ? strcmp(protocol, "mailto") ? "://" : ":" : NULL;

	// Add host delimiter if path is file only
	char *hd = (path[0] != HOST_DELIMITER[0]) && host ? HOST_DELIMITER : NULL;


	// Allocate persistent memory
	char *url = (char*)malloc(((protocol ? strlen(protocol):0) + (pd ? strlen(pd):0)
							+ (host ? strlen(host):0)  + (hd ? strlen(hd):0) + strlen(path) + 1) * sizeof(char));

	// Concatenate strings
	sprintf(url, "%s%s%s%s%s", (protocol ? protocol : ""), (pd ? pd : ""), (host ? host : ""), (hd ? hd : ""), path);

	return url;
}
