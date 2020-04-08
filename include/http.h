// HTTP configuration
#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP_VER   "HTTP/1.1"
#define PORT       80

// Flags
#define TRUNCATED       'T'
#define CONTENT_TYPE_NA 'M'

#define PRINTERR 0

int http_get(char *url, char *response, char *flag);
