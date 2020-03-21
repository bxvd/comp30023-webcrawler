extern const int MAX_URL_LENGTH;

typedef struct url_t URL;

URL parseURL(const char* url);
char* stringifyURL(char* path, ...);
