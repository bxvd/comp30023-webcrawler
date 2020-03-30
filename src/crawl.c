#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"
#include "http.h"

#define MAX_RESPONSE_LENGTH 100000

int crawl(const char *url) {

	FILE *output = fopen("response.html", "w");
	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));
	char* _url = (char*)malloc(strlen(url) * sizeof(char));
	memmove(_url, url, strlen(url));

	fprintf(stderr, "Starting crawl.\n");

	// Make GET request
	int status = http_get(_url, response);

	fprintf(output, "%s", response);

	free(response);
	fclose(output);

	fprintf(stderr, "Crawl finished.\n");

	return 0;
}
