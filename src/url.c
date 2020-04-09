/*
 * Source file: url.c
 * 
 * Provides URL handling functions.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 */

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
		element_start = strstr(url, LOCATION_DELIMITER);
		element_start += element_start ? strlen(LOCATION_DELIMITER) : 0;
	}

	element_start = element_start ? strstr(element_start, HOST_DELIMITER) : strstr(url, HOST_DELIMITER);

	// URL doesn't contain a path at all
	element_start = element_start ? element_start : DEFAULT_PATH;

	memmove(path, element_start, strlen(element_start));
}

/*
 * Function: get_directory
 * 
 * Takes in a URL string and extracts the directory.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the directory.
 */
void get_directory(char *url, char *directory) {

	assert(url && directory);

	// Take the path and cut off the file
	get_path(url, directory);

	// Find each '/' until we get to the last one
	char *end = url;
	while (end < (url + strlen(url)) && strstr(end + 1, HOST_DELIMITER)) {
		end = strstr(end + 1, HOST_DELIMITER);
	}

	// Move just the directory bytes
	memmove(directory, url, end - url);
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

/*
 * Function: sanitise
 * 
 * Rebuilds URL strings.
 * 
 * char *candidate: URL to be rebuilt.
 * char *sender:    URL where this request came from (used as the template).
 */
void sanitise(char *candidate, char *sender) {

	// Handle '//' URL beginnings
	if (strstr(candidate, LOCATION_DELIMITER) == candidate) {

		// Rebuild URL to be nicer
		char host[MAX_URL_LENGTH] = {0}, path[MAX_URL_LENGTH] = {0};
		get_host(candidate, host);
		get_path(candidate, path);

		char *_url = stringify_url(2, path, host);

		// Replace passed URL with a filled-out copy
		memset(candidate, 0, MAX_URL_LENGTH);
		memmove(candidate, _url, strlen(_url));

		free(_url);
	}

	// URL is a relative path and needs to be filled out
	if ((candidate[0] == HOST_DELIMITER[0] && candidate[1] != HOST_DELIMITER[0]) || strstr(candidate, HOST_DELIMITER) == NULL) {

		char *_url, directory[MAX_URL_LENGTH] = {0};

		get_directory(sender, directory);
		_url = stringify_url(2, candidate, directory);

		// Replace passed URL with a filled-out copy
		memset(candidate, 0, MAX_URL_LENGTH);
		memmove(candidate, _url, strlen(_url));

		free(_url);
	}
}
