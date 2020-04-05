#define MAX_URL_LENGTH 1000

#define DEFAULT_PROTOCOL   "http"
#define DEFAULT_PATH       "/"
#define PROTOCOL_DELIMITER "://"
#define HOST_DELIMITER     "/"
#define HOST_EL_DELIMITER  "."

typedef struct URL {
	char *path;
	char *host;
	char *protocol;
} URL;

int parse_url(char *url, URL *url_t);
void get_protocol(char *url, char *protocol);
void get_host(char *url, char *host);
void get_path(char *url, char *path);
char *stringify_url(int n, char *path, ...);
