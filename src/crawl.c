#include <stdio.h>
#include "http.h"

int crawl(char* url) {

	// Make GET request
	char* response = http_get(url);

	// Look at response
	fprintf(stderr, "Response:\n%s\n", response);

	return 0;
}
