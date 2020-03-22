#include <stdio.h>
#include <stdlib.h>
#include "http.h"

int crawl(char *url) {

	char *response = (char*)malloc(MAX_CONTENT_LENGTH * sizeof(char));

	// Make GET request
	int status = http_get(url, response);

	// Look at response
	fprintf(stderr, "Status: %d\nResponse:\n%s\n", status, response);

	return 0;
}
