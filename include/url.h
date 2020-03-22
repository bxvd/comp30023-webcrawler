typedef struct URL {
	char *path;
	char *host;
	char *protocol;
} URL;

int parse_url(char *url, URL *url_t);
char *stringify_url(int n, char *path, ...);
