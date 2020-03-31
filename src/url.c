#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct URL {
	char *path;
	char *host;
	char *protocol;
} URL;

#define INVALID_PATH_CHARS "?#%"
#define DEFAULT_PROTOCOL   "http"
#define DEFAULT_PATH       "/"
#define PROTOCOL_DELIMITER "://"
#define HOST_DELIMITER     "/"

#define INVALID_URL -7

/*
 * Function: parse_url
 * 
 * Parses a URL string into a struct to be used in request headers. Assumes a valid URL is passed in.
 * 
 * char *url:   URL string to be parsed.
 * char *url_t: struct that will be populated with the elements of the URL.
 * 
 * Returns int: Error code.
 */
int parse_url(char *url, URL *url_t) {

	/*
	 * Code below is just manouvering of pointers to substrings of the URL to get
	 * its elements, and copy the bytes into the struct.
	 */

	char *element_start = url, *element_end = NULL;
	int element_length;

	// Get protocol
	element_end = strstr(url, PROTOCOL_DELIMITER);

	if (element_end == NULL) {

		element_length = strlen(DEFAULT_PROTOCOL);
		element_start = DEFAULT_PROTOCOL;
		element_end = url;
	} else {

		element_length = element_end - element_start;
		element_end += strlen(PROTOCOL_DELIMITER);
	}

	url_t->protocol = (char*)malloc(element_length * sizeof(char));
	memmove(url_t->protocol, element_start, element_length);

	// Get host
	element_start = element_end;
	element_end = strstr(element_start, HOST_DELIMITER);
	element_length = element_end == NULL ? strlen(element_start) : element_end - element_start;

	url_t->host = (char*)malloc(element_length * sizeof(char));
	memmove(url_t->host, element_start, element_length);

	// Get path
	element_start = element_end == NULL ? DEFAULT_PATH : element_end;
	element_length = strlen(element_start);

	url_t->path = (char*)malloc(element_length * sizeof(char));
	memmove(url_t->path, element_start, element_length);
	
	return (url_t->protocol == NULL || url_t->host == NULL || url_t->path == NULL) ? INVALID_URL : 0;
}

/*
 * Function: stringify_url
 * 
 * Generates a valid URL string.
 * 
 *            int n:          Number of url arguments passed.
 *            char *path:     Path to resource (or the hostname if no path).
 * (optional) char *host:     Hostname.
 * (optional) char *protocol: Protocol to be used ('http' if none provided).
 * 
 * returns char*: URL string.
 */
char *stringify_url(int n, char *path, ...) {

	// Handle variable number of arguments
	va_list ap;
	va_start(ap, path);

	char *host = n >= 2 ? va_arg(ap, char*) : "\0";
	char *protocol = n == 3 ? va_arg(ap, char*) : "http";

	// Set protocol delimiter
	char *pd = strcmp(protocol, "mailto") ? "://" : ":";

	// Allocate persistent memory
	char *url = (char*)malloc(strlen(path) + strlen(host) + strlen(protocol) + strlen(pd));

	// Concatenate strings
	sprintf(url, "%s%s%s%s", protocol, pd, host, path);

	return url;
}
