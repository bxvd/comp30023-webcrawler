#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct URL {
	char *path;
	char *host;
	char *protocol;
} URL;

// Constants for finding substrings based on URL assumptions
#define URL_PROTOCOL_END 4
#define URL_HOST_START   7

#define INVALID_PATH_CHARS "?#%"
#define MIN_HOST_LENGTH    3

// Error codes
#define INVALID_PROTOCOL   1
#define INVALID_HOST       2
#define INVALID_PATH       3
#define INVALID_URL        4

/*
 * Function: parse_url
 * 
 * Parses a URL string into a struct to be used in request headers.
 * 
 * char *url:   URL string to be parsed.
 * char *url_t: struct that will be populated with the elements of the URL.
 * 
 * returns int: flag indicating validity of the URL string.
 */
int parse_url(char *url, URL *url_t) {

	/*
	 * Assumption:
	 * 	url will only contain strings of the format 'http://<host><path>'
	 */

	// Validity check on URL length
	if (strlen(url) <= URL_HOST_START) {

		fprintf(stderr, "Invalid URL\n");

		return INVALID_URL;
	}

	// Get protocol substring
	url_t->protocol = (char*)malloc(URL_PROTOCOL_END * sizeof(char));
	strncpy(url_t->protocol, url, URL_PROTOCOL_END);

	// Validity check on protocol
	if (strcmp(url_t->protocol, "http")) {

		fprintf(stderr, "Invalid protocol\n");

		return INVALID_PROTOCOL;
	}

	// Get path substring
	char *path = strchr(url + URL_HOST_START, '/');
	// No-path case
	path = path == NULL ? "/" : path;

	// Validity check on path form
	// The specified ignore cases are simple enough to avoid using regex
	for (int i = 0; i < (int)strlen(INVALID_PATH_CHARS); i++) {
		if (strchr(path, INVALID_PATH_CHARS[i])) {
			return INVALID_PATH;
		}
	}

	// Validity check on relative directories (./, ../) in path
	for (int i = 0; i < (int)strlen(path); i++) {
		if (path[i] == '.' && path[i + 1] == '/') {
			return INVALID_PATH;
		}
	}

	url_t->path = (char*)malloc(sizeof(*path));
	strcpy(url_t->path, path);

	/*
	 * Given validation cases above and project specifications, no host validation required.
	 */

	// Get host substring
	int host_len = (int)(path - url - URL_HOST_START);
	url_t->host = (char*)malloc(host_len * sizeof(char));
	strncpy(url_t->host, (url + URL_HOST_START), host_len);

	return 0;
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
 * returns char*: A URL string.
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
