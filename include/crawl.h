// Page as linked list node
typedef struct Page {
	char *location;
	char flag;
	int status;
	struct Page *head;
	struct Page *prev;
	struct Page *next;
} Page;

void crawl(char *url);

