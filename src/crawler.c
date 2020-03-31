#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawl.h"

#define ARGS_REQUIRED 2

#define ARGS_ERROR -7

int main(int argc, const char** argv) {

	if (argc != ARGS_REQUIRED) {
		fprintf(stderr, "Wrong number of args passed.\n");
		fprintf(stderr, "Usage:\n    %s [URL]\n", argv[0]);

		exit(ARGS_ERROR);
	}

	return crawl(argv[1]);
}
