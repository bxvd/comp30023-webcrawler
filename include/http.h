// HTTP configuration
#define USER_AGENT "bdaff"
#define CONNECTION "keep-alive"
#define ACCEPT     "text/html"
#define HTTP_VER   "HTTP/1.1"
#define PORT       80

int http_get(char *url, char *response);
