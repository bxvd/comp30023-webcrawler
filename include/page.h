// Doubly linked list representing the web pages
typedef struct Page {
	char *location;
	char flag;
	int status;
	struct Page *prev;
	struct Page *next;
} Page;

Page *get_page(char *url, Page *prev);
void destroy_page(Page *page, int r);
int exists(char *location, Page *page);
void add_page(Page *page);
