#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

int crawl(char *url) {

	char *response = NULL;

	// Make GET request
	int status = http_get(url, response);

	if (status < 0) {

		free(response);

		return status;
	}

	//fprintf(stderr, "Status: %d\nResponse:\n%s\n", status, response);

	free(response);

	return 0;
}
