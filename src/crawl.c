#include <stdio.h>
#include "http.h"

int crawl(char* url) {

	// GET request
	char* response = HTTPGet(url);

	// Look at response
	fprintf(stderr, "%s", response);

	return 0;
}
