#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_URL_LENGTH = 1000;

typedef struct url_t {
	char* path;
	char* host;
	char* protocol;
} URL;

URL parseURL(char* url) {

	struct url_t URL = {
		url,
		"",
		""
	};

	return URL;
}

char* stringifyURL(int n, char* path, ...) {

	// Handle variable number of arguments
	va_list ap;
	va_start(ap, path);

	char* host = n >= 2 ? va_arg(ap, char*) : "";
	char* protocol = n == 3 ? va_arg(ap, char*) : "http";

	// Allocate heap memory so as to not get freed from the stack
	int size = ((n == 3 ? 0 : 4) * sizeof(char) + (void*)ap - (void*)path);
	char* url = malloc(size);

	// Concatenate strings
	sprintf(url, "%s://%s%s", protocol, host, path);

	return url;
}
