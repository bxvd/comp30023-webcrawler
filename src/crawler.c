#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawl.h"

int main(int argc, const char** argv) {

	// Validate args
	if (argc != 2) {

		fprintf(stderr, "Usage:\n    %s [URL]\n", argv[0]);

		exit(1);
	}
	
	char* url = "";
	strcpy(url, argv[1]);

	return crawl(url);
}
