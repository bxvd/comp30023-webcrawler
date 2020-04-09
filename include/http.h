/*
 * Header file: http.h
 * 
 * Connects to and accesses web servers via HTTP GET.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 */

// HTTP configuration
#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP_VER   "HTTP/1.1"
#define AUTH       "Basic YmRhZmY6cGFzc3dvcmQ="
#define PORT       80

#define MAX_RESPONSE_LENGTH 100000

// Handler flags
#define OK                'O'
#define TRUNCATED         'T'
#define REDIRECTED        'R'
#define AUTH_REQUIRED     'A'
#define CONTENT_TYPE_NA   'M'
#define PERMANENT_FAILURE 'X'

/*
 * Function: http_get
 * 
 * Generates a HTTP GET header and captures the response.
 * 
 * char *url:      URL string of server.
 * char *response: Pointer to the location of where to put the response.
 * char *flag:     Flag for response handling (flags are defined in http.h).
 * 
 * Returns int: HTTP status code or error code.
 */
int http_get(char *url, char *response, char *flag);
