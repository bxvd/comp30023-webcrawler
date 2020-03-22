#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"

#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP       "HTTP/1.1"

#define MAX_URL_LENGTH     1000
#define MAX_REQUEST_LENGTH 1000

#define INVALID_URL -1

int http_get(char *url, char *response) {

	int status = 0;
	char *request = (char*)malloc(MAX_REQUEST_LENGTH * sizeof(char));
	char *protocol = HTTP;
	
	// Parse url string to URL struct
	URL *_url = (URL*)malloc(MAX_URL_LENGTH * sizeof(char));
	int valid_url = !parse_url(url, _url);

	if (!valid_url) {
		
		free(_url);

		return INVALID_URL;
	}

	// Generate request header
	sprintf(
		request,
		"GET %s %s\nHost: %s\nConnection: %s\nUser-Agent: %s\nAccept: %s\n",
		_url->path, protocol, _url->host, CONNECTION, USER_AGENT, ACCEPT
	);

	response = NULL;

	free(_url);

	// Just return request until implementation is ready
	return status;
}
