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

#define NO_LINKS     0
#define IGNORE       1
#define DO_NOT_CRAWL 0
#define SHOULD_CRAWL 1

/*
 * Adapted from find_links.cc at https://github.com/google/gumbo-parser/tree/master/examples
 * 
 * Function: link_search
 * 
 * Recursive HTML node tree traverser finding all hrefs.
 * 
 * char **links:    Array to store search results.
 * int *n:          Number of links stored in 'links' so far.
 * GumboNode *root: Document root node as a Gumbo struct.
 */
void link_search(char **links, int *n, GumboNode *node) {

	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}

	GumboAttribute *href;
	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
		fprintf(stderr, "Found link: %s\n", href->value);
		// Node is an <a> tag with a href value
		links[*n] = (char*)malloc(strlen(href->value) * sizeof(char));
		memmove(links[(*n)++], href->value, strlen(href->value));
	}

	// Traverse tree
	GumboVector *children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		link_search(links, n, children->data[i]);
	}
}

/*
 * Function: get_links
 * 
 * Dispatches tree traversal search on a response character string.
 *
 * char **links:   Array to store search results.
 * int *n:         Number of links stored in 'links' so far.
 * char *response: Server content.
 */
void get_links(char **links, int *n, char *response) {

	GumboOutput *output = gumbo_parse(response);

	link_search(links, n, output->root);

  gumbo_destroy_output(&kGumboDefaultOptions, output);
}

int should_crawl(char *proposed, char *current) {
	return SHOULD_CRAWL;
}

int crawl(char *url, char **pages, int *n) {

	//FILE *output = fopen("response.html", "w");
	const char path_ignore_strings[N_IGNORE_STRINGS][MAX_STRING_LENGTH] = PATH_IGNORE_STRINGS;

	// Ignore non-basic URLs
	for (int i = 0; i < N_IGNORE_STRINGS; i++) {
		if (strstr(url, path_ignore_strings[i]) != NULL) {
			return IGNORE;
		}
	}

	char *response = (char*)malloc(MAX_RESPONSE_LENGTH * sizeof(char));

	fprintf(stderr, "Fetching page: %s\n", url);

	// Make GET request
	int status = http_get(url, response);

	// Handle HTTP status codes
	if (status == 200) {
		fprintf(stderr, "Adding fetched page: %s\n", url);
		pages[*n] = (char*)malloc(strlen(url) * sizeof(char));
		memmove(pages[(*n)++], url, strlen(url));
	}

	// Get hrefs from document
	char **links = (char**)malloc(MAX_RESPONSE_LENGTH * sizeof(char));
	int n_links = 0;
	memset(links, 0, MAX_RESPONSE_LENGTH);
	get_links(links, &n_links, response);

	free(response);

	for (int i = 0; i < n_links; i++) {

		if (should_crawl(links[i], url)) {

			// URL string is not treated as a constant, do not mutate inside the array
			char *_url = (char*)malloc(strlen(links[i]) * sizeof(char));
			memmove(_url, links[i], strlen(links[i]));

			crawl(_url, pages, n);

			free(_url);
		}

		free(links[i]);
	}

	//fprintf(output, "%s", response);

	free(links);
	//fclose(output);

	return NO_LINKS;
}
