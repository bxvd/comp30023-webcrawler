#include <stdlib.h>
#include <string.h>
#include "page.h"
#include "url.h"
#include "crawl.h"

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
Page *get_page(char *url, Page *prev) {

	// Setup memory allocations
	Page *page = (Page*)malloc(sizeof(Page));
	page->location = (char*)malloc(MAX_URL_LENGTH * sizeof(char));
	memset(page->location, 0, MAX_URL_LENGTH);

	// Set values
	page->prev = prev;
	page->next = NULL;
	page->flag = 0;
	page->status = 0;
	memmove(page->location, url, strlen(url));

	return page;
}

/*
 * Function: destroy_page
 * 
 * Frees the memory used by a Page.
 * 
 * Page *page: Pointer to page to be destroyed.
 * int r:      Destroys children recursively if r evaluates to true.
 */
void destroy_page(Page *page, int r) {

	if (page->prev) {
		page->prev->next = page->next;
	}

	if (page->next) {

		if (r) {
			destroy_page(page->next, r);
		} else {
			page->next->prev = page->prev;
		}
	}

  free(page->location);
	free(page);
}

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
int exists(char *location, Page *page) {

	if (!strcmp(location, page->location)) {
		return IGNORE;
	}

	Page *prev = page;
	while ((prev = prev->prev)) {
		if (!strcmp(location, prev->location)) {
			return IGNORE;
		}
	}

	Page *next = page;
	while ((next = next->next)) {
		if (!strcmp(location, next->location)) {
			return IGNORE;
		}
	}

	return 0;
}

/*
 * Function: add_page
 * 
 * Adds a new page to the linked list that the passed Page is in.
 * 
 * Page *page: Page within linked list.
 */
void add_page(Page *page) {

	page->next = page->prev->next;
  page->prev->next = page;
  page->next ? page->next->prev = page : NULL;
}
