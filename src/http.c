#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"

#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP       "HTTP/1.1"

char* http_get(char* url) {

	char* request = (char*)malloc(1000 * sizeof(char));
	char* protocol = "";

	// Get URL struct of the url string
	URL* _url = parse_url(url);

	// Get header request ver of protocol
	if (!strcmp(_url->protocol, "http")) {
		protocol = HTTP;
	}

	// Generate request header
	sprintf(
		request,
		"GET %s %s\nHost: %s\nConnection: %s\nUser-Agent: %s\nAccept: %s\n",
		_url->path, protocol, _url->host, CONNECTION, USER_AGENT, ACCEPT
	);

	free(_url);

	return request;
}
