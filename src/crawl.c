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

#define PATH_IGNORE_STRINGS {"?", "#", "%", "./"}
#define N_IGNORE_STRINGS    4
#define MAX_STRING_LENGTH   2

#define MAX_RESPONSE_LENGTH 100000

#define NO_LINKS  0
#define IGNORE    1
#define RECURSIVE 1

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

	while (r && page->next) {
		destroy_page(page->next, r);
	}

	free (page);
}

/*
 * Function: find_links
 * 
 * Recursive HTML node tree traverser finding all hrefs.
 * 
 * GumboNode *node: HTML node as a Gumbo struct.
 */
void find_links(GumboNode *node, FILE *links) {

	// End of document
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}

	// Current node is a href
	GumboAttribute *href;
	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {

		fprintf(links, "%s\n", href->value);
	}

	// Current node is not a href, try its children
	GumboVector *children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		find_links((GumboNode*)children->data[i], links);
	}
}

void crawl(char *url) {

	FILE *response_output = fopen("output.html", "w");
	FILE *links_output = fopen("links.txt", "w");

	// Setup memory
	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));

	Page *head = get_page(url, NULL);
	head->status = http_get(head->location, response, &head->flag);
	// Page *head = (Page*)malloc(sizeof(Page));

	// if (status == 200) {

	// 	// Setup first page
	// 	head->head = head;
	// 	head->prev = NULL;
	// 	head->next = NULL;
	// 	head->location = url;
	// 	head->status = status;
	// }

	GumboOutput *parsed_output = gumbo_parse(response);
	find_links(parsed_output->root, links_output);
	gumbo_destroy_output(&kGumboDefaultOptions, parsed_output);

	fprintf(stderr, "Status: %d\n", head->status);
	fprintf(response_output, "%s\n", response);

	destroy_page(head, RECURSIVE);

	free(response);
	fclose(response_output);
	fclose(links_output);
}


// int should_ignore(char *url, char *host, char **pages, int n) {

// 	const char path_ignore_strings[N_IGNORE_STRINGS][MAX_STRING_LENGTH] = PATH_IGNORE_STRINGS;

// 	// Ignore non-basic URLs
// 	for (int i = 0; i < N_IGNORE_STRINGS; i++) {
// 		if (strstr(url, path_ignore_strings[i]) != NULL) {
// 			return IGNORE;
// 		}
// 	}

// 	// Fill out URLs that are path or file only
// 	if (strstr(url, host) == NULL) {

// 		char *_url = stringify_url(2, url, host);

// 		memset(url, 0, MAX_URL_LENGTH);
// 		memmove(url, _url, MAX_URL_LENGTH);

// 		free(_url);

// 		return should_ignore(url, host, pages, n);
// 	}

// 	for (int i = 0; i < n - 1; i++) {
// 		if (!strcmp(url, pages[n])) {
// 			return IGNORE;
// 		}
// 	}

// 	return 0;
// }

// /*
//  * Adapted from find_links.cc at https://github.com/google/gumbo-parser/tree/master/examples
//  * 
//  * Function: link_search
//  * 
//  * Recursive HTML node tree traverser finding all hrefs.
//  * 
//  * char **links:    Array to store search results.
//  * int *n:          Number of links stored in 'links' so far.
//  * GumboNode *root: Document root node as a Gumbo struct.
//  */
// void crawl_links(GumboNode *node, char *url, char **pages, int *n) {

// 	if (node->type != GUMBO_NODE_ELEMENT) {
// 		return;
// 	}

// 	char host[MAX_URL_LENGTH] = {0};
// 	get_host(url, host);

// 	GumboAttribute *href;
// 	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {

// 		char *link = (char*)malloc(MAX_URL_LENGTH * sizeof(char));
// 		memset(link, 0, MAX_URL_LENGTH);
// 		memmove(link, href->value, strlen(href->value));

// 		fprintf(stderr, "Found link: %s\n", link);
// 		if (!should_ignore(link, host, pages, *n)) {
// 			crawl(link, pages, n);
// 		}
// 	}

// 	// Traverse tree
// 	GumboVector *children = &node->v.element.children;
// 	for (unsigned int i = 0; i < children->length; ++i) {
// 		crawl_links(children->data[i], url, pages, n);
// 	}
// }

// int crawl(char *url, char **pages, int *n) {

// 	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));

// 	fprintf(stderr, "Fetching page: %s\n", url);

// 	// Make GET request
// 	int status = http_get(url, response);

// 	// Handle HTTP status codes
// 	if (status == 200) {
// 		fprintf(stderr, "Adding fetched page: %s\n", url);
// 		pages[*n] = (char*)malloc(strlen(url) * sizeof(char));
// 		memmove(pages[(*n)++], url, strlen(url));
// 	}

// 	// Parse HTML
// 	GumboOutput *output = gumbo_parse(response);

// 	free(response);

// 	crawl_links(output->root, url, pages, n);

// 	gumbo_destroy_output(&kGumboDefaultOptions, output);

// 	return NO_LINKS;
// }
