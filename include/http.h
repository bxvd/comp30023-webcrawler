// HTTP configuration
#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP_VER   "HTTP/1.1"
#define PORT       80

// Flags
#define OK                'O'
#define TRUNCATED         'T'
#define REDIRECTED        'R'
#define CONTENT_TYPE_NA   'M'
#define PERMANENT_FAILURE 'X'

#define PRINTERR 1

int http_get(char *url, char *response, char *flag);
