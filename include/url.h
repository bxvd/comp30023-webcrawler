typedef struct URL {
	char* path;
	char* host;
	char* protocol;
} URL;

URL* parse_url(char* url);
char* stringify_url(char* path, ...);
