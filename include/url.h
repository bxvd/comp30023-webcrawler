/*
 * Header file: url.h
 * 
 * Provides URL handling functions and constants.
 * 
 * Author: bdaff@student.unimelb.edu.au (Brodie Daff)
 */

#define MAX_URL_LENGTH 1024

#define DEFAULT_PROTOCOL   "http"
#define DEFAULT_PATH       "/"

#define PROTOCOL_DELIMITER "://"
#define HOST_DELIMITER     "/"
#define HOST_EL_DELIMITER  "."
#define LOCATION_DELIMITER "//"

#define PATH_IGNORE_CHARS "?#%"
#define RELATIVE_DIR      "./"

/*
 * Function: get_protocol
 * 
 * Takes in a URL string and extracts the protocol.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the protocol.
 */
void get_protocol(char *url, char *protocol);

/*
 * Function: get_host
 * 
 * Takes in a URL string and extracts the hostname.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the hostname.
 */
void get_host(char *url, char *host);

/*
 * Function: get_path
 * 
 * Takes in a URL string and extracts the path.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the path.
 */
void get_path(char *url, char *path);

/*
 * Function: get_directory
 * 
 * Takes in a URL string and extracts the directory.
 * 
 * char *url:  URL string.
 * char *host: Pointer to memory location for the directory.
 */
void get_directory(char *url, char *directory);

/*
 * Function: stringify_url
 * 
 * Generates a valid URL string.
 * 
 *            int n:          Number of url arguments passed.
 *            char *path:     Path to resource (or the the URL itself to be stringified).
 * (optional) char *host:     Hostname.
 * (optional) char *protocol: Protocol to be used ('http' if none provided).
 * 
 * returns char*: URL string.
 */
char *stringify_url(int n, char *path, ...);

/*
 * Function: sanitise
 * 
 * Rebuilds URL strings.
 * 
 * char *candidate: URL to be rebuilt.
 * char *sender:    URL where this request came from (used as the template).
 */
void sanitise(char *candidate, char *sender);
