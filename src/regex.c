/*
 * Function: handle_regex_error
 * 
 * Generates user-friendly error message if regex has a fatal error and ends the program.
 * 
 * int errorcode:     Error code resulting from compiling regex.
 * regex_t *compiled: Compiled regex.
 */
/*
void handle_regex_error(int errorcode, regex_t *compiled) {

	if (!errorcode) {

		// Generate error message
		size_t len = regerror(errorcode, compiled, NULL, 0);
		char *buffer = (char*)malloc(len);
		regerror(errorcode, compiled, buffer, len);

		fprintf(stderr, "Regex error: %s\n", buffer);

		exit(1);
	}
}

// Where regex will compile to
regex_t* compiled = (regex_t*)malloc(sizeof(regex_t));

// Compile regex and handle compilation errors
handle_regex_error(regcomp(compiled, "", REG_ICASE), compiled);
*/