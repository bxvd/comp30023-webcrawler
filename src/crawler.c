#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawl.h"

#define ARGS_REQUIRED 2

#define ARGS_ERROR 1

/*
 * Function: validate_args
 * 
 * Ensures correct command line args are passed.
 * 
 * int argc:          main's argc.
 * const char** argv: main's argv.
 */
void validate_args(int argc, const char** argv) {

	if (argc != ARGS_REQUIRED) {
		fprintf(stderr, "Wrong number of args passed.\n");
		fprintf(stderr, "Usage:\n    %s [URL]\n", argv[0]);

		exit(ARGS_ERROR);
	}
}

int main(int argc, const char** argv) {

	validate_args(argc, argv);

	return crawl(argv[1]);
}
