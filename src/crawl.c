#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

#define MAX_RESPONSE_LENGTH 100000

int crawl(char *url) {

	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));
	memset(response, 0, MAX_RESPONSE_LENGTH);

	// Make GET request
	int status = http_get(url, response);

	// Look at response
	//fprintf(stderr, "Status: %d\nResponse:\n%s\n", status, response);

	return 0;
}
