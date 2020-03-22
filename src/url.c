#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct URL {
	char* path;
	char* host;
	char* protocol;
} URL;

/*
 * Function: parse_url
 * 
 * UNFINISHED
 */
URL* parse_url(char* url) {

	// Allocate mem for struct
	URL* _url = (URL*)malloc((strlen(url) + 3) * sizeof(char));

	// Allocate mem for fields
	_url->path = (char*)malloc(11 * sizeof(char));
	_url->host = (char*)malloc(11 * sizeof(char));
	_url->protocol = (char*)malloc(4 * sizeof(char));

	// Simple copy of url string to struct fields
	strcpy(_url->path, (url + 18));
	strncpy(_url->host, (url + 7), 11 * sizeof(char));
	strncpy(_url->protocol, url, 4 * sizeof(char));

	return _url;
}

/*
 * Function: stringify_url
 * 
 * Generates a valid URL string.
 * 
 *            int n:          Number of url arguments passed.
 *            char* path:     Path to resource (i.e. '/a/b.html' or 'google.com/a/b.html').
 * (optional) char* host:     Hostname (i.e. 'google.com').
 * (optional) char* protocol: Protocol to be used ('http' by default).
 */
char* stringify_url(int n, char* path, ...) {

	// Handle variable number of arguments
	va_list ap;
	va_start(ap, path);

	char* host = n >= 2 ? va_arg(ap, char*) : "\0";
	char* protocol = n == 3 ? va_arg(ap, char*) : "http";

	// Set protocol delimiter
	char* pd = strcmp(protocol, "mailto") ? "://" : ":";

	// Allocate persistent memory
	char* url = (char*)malloc(strlen(path) + strlen(host) + strlen(protocol) + strlen(pd));

	// Concatenate strings
	sprintf(url, "%s%s%s%s", protocol, pd, host, path);

	return url;
}
