/*
 * header file: page.h
 * 
 * Provides functionality to a doubly linked list data
 * structure of web pages through a Page struct.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 */

// Doubly linked list representing the web pages
typedef struct Page {
	char *location;
	char flag;
	int status;
	struct Page *prev;
	struct Page *next;
} Page;

/*
 * Function: get_page
 * 
 * Initialises a Page struct.
 * 
 * char *url:  URL string to page location.
 * Page *prev: The previous node in the doubly linked list data structure.
 * 
 * Returns Page*: New memory-allocated Page.
 */
Page *get_page(char *url, Page *prev);

/*
 * Function: destroy_page
 * 
 * Frees the memory used by a Page.
 * 
 * Page *page: Pointer to page to be destroyed.
 * int r:      Destroys children recursively if r evaluates to true.
 */
void destroy_page(Page *page, int r);

/*
 * Function: exists
 * 
 * Checks if a location exists in the linked list of pages.
 * 
 * char *location: URL to be checked.
 * Page *page:     Page within a linked list.
 * 
 * Returns int: 0 if no match found or 1 if there is.
 */
int exists(char *location, Page *page);

/*
 * Function: add_page
 * 
 * Adds a new page to the linked list that the passed Page is in.
 * 
 * Page *page: Page within linked list.
 */
void add_page(Page *page);
