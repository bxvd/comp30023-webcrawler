/*
 * Source file: crawl.c
 * 
 * Handles and dispatches all crawl logic and most of the memory
 * handling for the crawler program.
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
#include "page.h"

#include "gumbo.h"

#define PATH_IGNORE_CHARS "?#%"
#define RELATIVE_DIR      "./"

#define MAX_RESPONSE_LENGTH 100000

#define NO_LINKS  0
#define RECURSIVE 1
#define SINGLE    0

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

	// Ignore invalid directories
	char path[MAX_URL_LENGTH] = {0};
	get_path(url, path);

	if (strstr(path, LOCATION_DELIMITER)) {
		return IGNORE;
	}

	// Only visit pages if all but the first component of the host are the same
	char candidate[MAX_URL_LENGTH] = {0};
	get_host(url, candidate);

	// Compare host strings from where the first '.' is found
	if (strstr(host, HOST_EL_DELIMITER) && strstr(candidate, HOST_EL_DELIMITER)) {
		if (strcmp(strstr(host, HOST_EL_DELIMITER), strstr(candidate, HOST_EL_DELIMITER))) {
			return IGNORE;
		}
	} else if (strcmp(host, candidate)) {
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
 * Page *page:      Page in a linked list.
 */
void find_links(GumboNode *node, Page *page) {

	// End of document
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}

	// Current node is a href
	GumboAttribute *href;
	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {

		// if (PRINTERR) {
		// 	fprintf(stderr, "href: %s\n", href->value);
		// }

		// Get elements of the href value to work with
		char location[MAX_URL_LENGTH] = {0}, host[MAX_URL_LENGTH] = {0};
		memmove(location, href->value, strlen(href->value));

		sanitise(location, page->location);

		// if (PRINTERR) {
		// 		fprintf(stderr, "Sanitised: %s\n", location);
		// 	}

		get_host(page->location, host);

		if (!ignore_link(location, host) && !exists(location, page)) {

			if (PRINTERR) {
				fprintf(stderr, "Adding: %s\n", location);
			}

			add_page(get_page(location, page));
		}
	}

	// Current node is not a href, try its children
	GumboVector *children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		find_links((GumboNode*)children->data[i], page);
	}
}

/*
 * Function: parse
 * 
 * Dispatches HTML parsing and linked list handling.
 * 
 * char *response: HTML content response.
 * Page *page:     Page in a linked list.
 */
void parse(char *response, Page *page) {

	GumboOutput *parsed_output = gumbo_parse(response);
	find_links(parsed_output->root, page);
	gumbo_destroy_output(&kGumboDefaultOptions, parsed_output);
}

void crawl(char *url) {

	// Setup memory
	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));
	Page *page, *head = get_page(url, NULL);

	if (PRINTERR) {
		fprintf(stderr, "Visiting %s\n", head->location);
	} else {
		fprintf(stdout, "%s\n", head->location);
	}

	head->status = http_get(head->location, response, &head->flag);

	if (head->flag == OK) {
		parse(response, head);
	}

	// Traverse and generate linked list
	page = head->next;
	while (page) {

		if (PRINTERR) {
			fprintf(stderr, "Visiting %s\n", page->location);
		} else {
			fprintf(stderr, "%s\n", page->location);
		}

		page->status = http_get(page->location, response, &page->flag);

		// Flags used to categories the statuses and responses
		switch (page->flag) {

			// Truncated AND rettempted, don't parse
			case TRUNCATED:
				break;
			
			// Not accepting this content type
			case CONTENT_TYPE_NA:
				break;
			
			// All other scenarios are accounted for within HTTP handler
			default:
				parse(response, page);
				break;
		}

		page = page->next;
	}

	// Re-attempt temporary errors
	// page = head->next;
	// while (page) {

	// 	if (PRINTERR) {
	// 		fprintf(stderr, "Visiting %s\n", page->location);
	// 	} else {
	// 		fprintf(stderr, "%s\n", page->location);
	// 	}


	// 	page->status = http_get(page->location, response, &page->flag);

	// 	if (page->flag == OK) {

	// 		parse(response, page);
	// 		page = page->next;

	// 		if (page) {
	// 			destroy_page(page->prev, 0);
	// 		}
	// 	}
	// }

	destroy_page(head, RECURSIVE);

	free(response);
}
