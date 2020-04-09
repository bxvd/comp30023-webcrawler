/*
 * Header file: crawl.h
 * 
 * Handles and dispatches all crawl logic and memory
 * handling for the crawler program.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 */
#define IGNORE 1

/*
 * Function: crawl
 * 
 * Performs a HTTP GET crawl on a URL passed and outputs each link to stderr.
 * 
 * char *url: URL to begin crawl from.
 * 
 * Returns int: Error code.
 */
int crawl(char *url);
