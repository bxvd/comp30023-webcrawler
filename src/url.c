#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"

#define INVALID_URL -5

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

	url_t = (URL*)malloc(MAX_URL_LENGTH * sizeof(char));
	memset(url_t, 0, MAX_URL_LENGTH);

	//fprintf(stderr, "URL: %s\n", url);

	/*
	 * Code below is just manouvering of pointers to substrings of the URL to get
	 * its elements, and copy the bytes into the struct.
	 */

	char *element_start = url, *element_end = NULL;
	int element_length;

	// Get protocol
	element_end = strstr(element_start, PROTOCOL_DELIMITER);

	if (element_end == NULL) {

		element_length = strlen(DEFAULT_PROTOCOL);
		element_start = DEFAULT_PROTOCOL;
		element_end = url;
	} else {

		element_length = element_end - element_start;
		element_end += strlen(PROTOCOL_DELIMITER);
	}

	//fprintf(stderr, "    Start: %s, length: %d\n", element_start, element_length);

	url_t->protocol = (char*)malloc(element_length * sizeof(char));
	memmove(url_t->protocol, element_start, element_length);
	//fprintf(stderr, "  protocol: %s\n", url_t->protocol);

	// Get host
	element_start = element_end;
	element_end = strstr(element_start, HOST_DELIMITER);
	element_length = element_end == NULL ? strlen(element_start) : element_end - element_start;

	//fprintf(stderr, "    Start: %s, length: %d\n", element_start, element_length);

	url_t->host = (char*)malloc(element_length * sizeof(char));
	memmove(url_t->host, element_start, element_length);
	//fprintf(stderr, "  host: %s\n", url_t->host);

	// Get path
	element_start = element_end == NULL ? DEFAULT_PATH : element_end;
	element_length = strlen(element_start);

	//fprintf(stderr, "    Start: %s, length: %d\n", element_start, element_length);

	url_t->path = (char*)malloc(element_length * sizeof(char));
	memmove(url_t->path, element_start, element_length);
	//fprintf(stderr, "  path: %s\n", url_t->path);
	
	return (url_t->protocol == NULL || url_t->host == NULL || url_t->path == NULL) ? INVALID_URL : 0;
}

void get_protocol(char *url, char *protocol) {

	char *element_end = strstr(url, PROTOCOL_DELIMITER);
	char *element_start = element_end == NULL ? DEFAULT_PROTOCOL : url;
	int element_length = element_end == NULL ? strlen(DEFAULT_PROTOCOL) : element_end - url;

	memmove(protocol, element_start, element_length);
}

void get_host(char *url, char *host) {

	char *element_start = strstr(url, PROTOCOL_DELIMITER);
	element_start = element_start == NULL ? url : element_start + strlen(PROTOCOL_DELIMITER);

	char *element_end = strstr(element_start, HOST_DELIMITER);
	element_end = element_end == NULL ? (url + strlen(url) - 1) : element_end;

	memmove(host, element_start, element_end - element_start);
}

void get_path(char *url, char *path) {

	char *element_start = strstr(url, PROTOCOL_DELIMITER);
	element_start = element_start == NULL ? url : strstr(element_start + strlen(PROTOCOL_DELIMITER), HOST_DELIMITER);
	element_start = element_start == NULL ? DEFAULT_PATH : element_start + strlen(HOST_DELIMITER) - 1;

	memmove(path, element_start, strlen(element_start));
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

	// Add host delimiter if path is file only
	char *hd = path[0] != HOST_DELIMITER[0] ? HOST_DELIMITER : "";

	// Allocate persistent memory
	char *url = (char*)malloc((strlen(path) + strlen(hd) + strlen(host) + strlen(protocol) + strlen(pd)) * sizeof(char));

	// Concatenate strings
	sprintf(url, "%s%s%s%s%s", protocol, pd, host, hd, path);

	return url;
}
