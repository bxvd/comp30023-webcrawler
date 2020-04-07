/*
 * Source file: crawl.c
 * 
 * Handles and dispatches all crawl logic and memory handling for the
 * crawler program.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 * 
 * Acknowledgements:
 * * Function find_links is adapted from find_links.cc at https://github.com/google/gumbo-parser/tree/master/examples
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawl.h"
#include "url.h"
#include "http.h"
#include "gumbo.h"

#define PATH_IGNORE_CHARS "?#%"
#define RELATIVE_DIR      "./"

#define MAX_RESPONSE_LENGTH 100000

#define SUBMISSION 1

#define NO_LINKS  0
#define IGNORE    1
#define RECURSIVE 1
#define SINGLE    0

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
 * int r:      Destroys children recursively if r doesn't evaluate to 0.
 */
void destroy_page(Page *page, int r) {

	free(page->location);

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

	free(page);
}

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
 * Function: ignore_link
 * 
 * Returns a flag indicating whether a link should be ignored from the crawl.
 * 
 * char *url:  URL to be evaluated.
 * char *host: Original host.
 * 
 * Returns int: Evaluative flag.
 */
int ignore_link(char *url, char *host) {

	// Ignore URLs with functional characters in the path
	for (int i = 0; i < (int)strlen(PATH_IGNORE_CHARS); i++) {
		if (strchr(url, PATH_IGNORE_CHARS[i])) {
			return IGNORE;
		}
	}

	// Ignore relative directories
	if (strstr(url, RELATIVE_DIR)) {
		return IGNORE;
	}

	// Handle '//' URL beginnings
	if (strstr(url, LOCATION_DELIMITER) == url) {

		// Rebuild URL to be nicer
		char host[MAX_URL_LENGTH] = {0}, path[MAX_URL_LENGTH] = {0};
		get_host(url, host);
		get_path(url, path);

		char *_url = stringify_url(2, host, path);

		// Replace passed URL with a filled-out copy
		memset(url, 0, MAX_URL_LENGTH);
		memmove(url, _url, strlen(_url));

		free(_url);
	}

	// Ignore invalid directories
	char path[MAX_URL_LENGTH] = {0};
	get_path(url, path);

	if (strstr(path, LOCATION_DELIMITER)) {
		return IGNORE;
	}

	// URL is a relative path and needs to be filled out
	if (url[0] == '/' || strstr(url, HOST_DELIMITER) == NULL) {

		char *_url = stringify_url(2, url, host);

		// Replace passed URL with a filled-out copy
		memset(url, 0, MAX_URL_LENGTH);
		memmove(url, _url, strlen(_url));

		free(_url);
	}

	// Only visit pages if all but the first component of the host are the same
	char candidate[MAX_URL_LENGTH] = {0};
	get_host(url, candidate);

	if (!SUBMISSION) {

		// Handle cases where the original host is only 'name.TLD'
		int n_host = 0, n_candidate = 0;
		char *_host = host, *_candidate = candidate;

		while ((_host = strstr(_host, HOST_EL_DELIMITER))) {
			_host++;
			n_host++;
		}

		while ((_candidate = strstr(_candidate, HOST_EL_DELIMITER))) {
			_candidate++;
			n_candidate++;
		}

		// Compare host strings from where the first '.' is found
		if (strcmp(n_host <= 1 ? host : strstr(host, HOST_EL_DELIMITER) + 1, n_candidate <= 1 ? candidate : strstr(candidate, HOST_EL_DELIMITER) + 1)) {
			return IGNORE;
		}
	} else {

		// Compare host strings from where the first '.' is found
		if (strcmp(strstr(host, HOST_EL_DELIMITER), strstr(candidate, HOST_EL_DELIMITER))) {

			return IGNORE;
		}
	}

	return 0;
}

void add_page(Page *page) {

	while (page->prev->next) {
		page->prev = page->prev->next;
	}

	page->prev->next = page;
	page->next = NULL;
}

/*
 * Function: find_links
 * 
 * Recursive HTML node tree traverser finding all hrefs.
 * 
 * GumboNode *node: HTML node as a Gumbo struct.
 */
void find_links(GumboNode *node, Page *page) {

	// End of document
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}

	// Current node is a href
	GumboAttribute *href;
	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {

		// Get elements of the href value to work with
		char location[MAX_URL_LENGTH] = {0}, host[MAX_URL_LENGTH] = {0};
		memmove(location, href->value, strlen(href->value));
		get_host(page->location, host);

		if (!ignore_link(location, host) && !exists(location, page)) {
			add_page(get_page(location, page));
		}
	}

	// Current node is not a href, try its children
	GumboVector *children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		find_links((GumboNode*)children->data[i], page);
	}
}

void parse(char *response, Page *page) {

	GumboOutput *parsed_output = gumbo_parse(response);
	find_links(parsed_output->root, page);
	gumbo_destroy_output(&kGumboDefaultOptions, parsed_output);
}

void crawl(char *url) {

	// Setup memory
	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));

	Page *page, *head = get_page(url, NULL);
	head->status = http_get(head->location, response, &head->flag);

	if (PRINTERR) {
		fprintf(stderr, "Visiting ");
	}
	fprintf(stdout, "%s\n", head->location);
	parse(response, head);

	page = head;

	while ((page = page->next)) {

		if (PRINTERR) {
			fprintf(stderr, "Visiting ");
		}
		fprintf(stdout, "%s\n", page->location);

		page->status = http_get(page->location, response, &page->flag);

		parse(response, page);
	}

	destroy_page(head, RECURSIVE);

	free(response);
}
