#include <stdio.h>
#include <stdlib.h>
#include "url.h"

#define USER_AGENT "bdaff"

char* get_user_agent() {
	return USER_AGENT;
}

char* http_get(char* url) {

	// Get URL struct of the url string
	URL* _url = parse_url(url);

	fprintf(stderr, "Protocol: %s\nHost: %s\nPath: %s\n", _url->protocol, _url->host, _url->path);

	free(_url);

	return url;
}
