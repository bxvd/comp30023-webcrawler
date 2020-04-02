#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawl.h"
#include "url.h"

#define ARGS_REQUIRED     2
#define MAX_PAGES_FETCHED 100

#define ARGS_ERROR -7

int main(int argc, const char **argv) {

	if (argc != ARGS_REQUIRED) {
		fprintf(stderr, "Wrong number of args passed.\n");
		fprintf(stderr, "Usage:\n    %s [URL]\n", argv[0]);

		exit(ARGS_ERROR);
	}

	char **pages = (char**)malloc(MAX_PAGES_FETCHED * MAX_URL_LENGTH * sizeof(char));
	char *url = (char*)malloc(MAX_URL_LENGTH * sizeof(char));
	int n = 0;

	memset(pages, 0, MAX_PAGES_FETCHED * MAX_URL_LENGTH);
	memset(url, 0, MAX_URL_LENGTH);
	memmove(url, argv[1], strlen(argv[1]));
	
	int error_code = crawl(url, pages, &n);

	while (n > 0) {
		free(pages[--n]);
	}

	free(pages);
	free(url);

	return error_code;
}
