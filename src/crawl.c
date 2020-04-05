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

int ignore_link(char *url, char *host) {

	// Ignore URLs with functional characters in the path
	for (int i = 0; i < strlen(PATH_IGNORE_CHARS); i++) {
		if (strchr(url, PATH_IGNORE_CHARS[i]) != NULL) {
			return IGNORE;
		}
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
	char *candidate[MAX_URL_LENGTH] = {0};
	get_host(url, candidate);

	// Compare host strings from where the first '.' is found
	if (!strcmp(strstr(candidate, HOST_EL_DELIMITER), strstr(host, HOST_EL_DELIMITER))) {
		return IGNORE;
	}

	return 0;
}

/*
 * Function: find_links
 * 
 * Recursive HTML node tree traverser finding all hrefs.
 * 
 * GumboNode *node: HTML node as a Gumbo struct.
 */
void find_links(GumboNode *node, Page *page, FILE *links) {

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

		fprintf(links, "%s > ", href->value);

		if (!ignore_link(location, host)) {
			fprintf(links, "%s\n", location);
		} else {
			fprintf(links, "IGNORE\n");
		}
	}

	// Current node is not a href, try its children
	GumboVector *children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		find_links((GumboNode*)children->data[i], page, links);
	}
}

void crawl(char *url) {

	FILE *response_output = fopen("output.html", "w");
	FILE *links_output = fopen("links.txt", "w");

	// Setup memory
	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));

	Page *head = get_page(url, NULL);
	head->status = http_get(head->location, response, &head->flag);

	GumboOutput *parsed_output = gumbo_parse(response);
	find_links(parsed_output->root, head, links_output);
	gumbo_destroy_output(&kGumboDefaultOptions, parsed_output);

	fprintf(stderr, "Status: %d\n", head->status);
	fprintf(response_output, "%s\n", response);

	destroy_page(head, RECURSIVE);

	free(response);
	fclose(response_output);
	fclose(links_output);
}
