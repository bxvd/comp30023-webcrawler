#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawl.h"
#include "url.h"

#define ARGS_REQUIRED     2
#define MAX_PAGES_FETCHED 100

#define ARGS_ERROR -7

int main(int argc, const char **argv) {

	// Check correct usage
	if (argc != ARGS_REQUIRED) {
		fprintf(stderr, "Wrong number of args passed.\n");
		fprintf(stderr, "Usage:\n    %s [URL]\n", argv[0]);

		exit(ARGS_ERROR);
	}

	// Copy URL arg before working with it
	char url[MAX_URL_LENGTH] = {0};
	memmove(url, argv[1], strlen(argv[1]));

	return crawl(url);
}
