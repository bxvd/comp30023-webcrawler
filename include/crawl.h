// Doubly linked list representing the web pages
typedef struct Page {
	char *location;
	char flag;
	int status;
	struct Page *prev;
	struct Page *next;
} Page;

void crawl(char *url);
