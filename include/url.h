#define MAX_URL_LENGTH 1000

typedef struct URL {
	char *path;
	char *host;
	char *protocol;
} URL;

int parse_url(char *url, URL *url_t);
char *get_host(char *url);
char *stringify_url(int n, char *path, ...);
