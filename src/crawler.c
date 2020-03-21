#include <stdio.h>
#include "crawl.h"

int main(int argc, char const **argv) {

	// Validate args
	if (argc != 2) {

		fprintf(stderr, "Usage:\n    %s [URL]\n", argv[0]);

		exit(1);
	}
	
	const char *url = argv[1];

	fprintf(stderr, "Fetching %s\n", url);

	return crawl(url);
}
