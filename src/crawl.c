#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"
#include "http.h"
#include "gumbo.h"

#define INVALID_PATH_STRINGS ["?", "#", "%", "./"]
#define N_INVALID_STRINGS    4

#define MAX_RESPONSE_LENGTH 100000
#define FLAGS_LENGTH        10

int crawl(const char *url) {

	FILE *output = fopen("response.html", "w");
	char flags[FLAGS_LENGTH];
	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));
	char *_url = (char*)malloc(strlen(url) * sizeof(char));
	memmove(_url, url, strlen(url));

	fprintf(stderr, "Starting crawl.\n");

	// Ignore non-basic URLs
	for (int i = 0; i < N_INVALID_STRINGS; i++) {

	}

	// Make GET request
	int status = http_get(_url, response, flags);

	GumboOutput *gumbo_output = gumbo_parse(response);
  gumbo_destroy_output(&kGumboDefaultOptions, gumbo_output);

	fprintf(output, "%s", response);

	free(response);
	fclose(output);

	fprintf(stderr, "Crawl finished.\n");

	return 0;
}
