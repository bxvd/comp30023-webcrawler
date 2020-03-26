#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "url.h"
#include "client.h"

#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP       "HTTP/1.1"
#define PORT       80

#define END_OF_HEADER "\r\n\r\n"

#define MAX_URL_LENGTH    1000
#define MAX_HEADER_LENGTH 256

#define INVALID_URL      -4
#define INVALID_RESPONSE -5

int parse_header(char *response) {

	// Find pointer to end-of-header marker
	char *end = strstr(response, END_OF_HEADER);

	if (end == NULL) {
		return INVALID_RESPONSE;
	}

	// Extract the HTTP status
	char status[3];
	strncpy(status, strchr(response, ' '), sizeof(status));

	return atoi(status);
}

int http_get(char *url, char *response) {

	char header[MAX_HEADER_LENGTH];
	memset(header, 0, MAX_HEADER_LENGTH);
	
	// Parse url string
	URL *_url = (URL*)malloc(MAX_URL_LENGTH * sizeof(char));

	if (parse_url(url, _url)) {
		
		free(_url);

		return INVALID_URL;
	}

	// Generate request header
	snprintf(
		header,
		sizeof(header),
		"GET %s %s\r\nHost: %s\r\nConnection: %s\r\nCache-Control: max-age=0\r\nUser-Agent: %s\r\nAccept: %s\r\n\r\n",
		_url->path, HTTP, _url->host, CONNECTION, USER_AGENT, ACCEPT
	);

	int errcode = request(_url->host, PORT, header, response);

	free(_url);

	return (errcode ? errcode : parse_header(response));
}
