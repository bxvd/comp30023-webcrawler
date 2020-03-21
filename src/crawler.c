#include <stdio.h>

int main(int argc, char const *argv[]) {
	
	const char *filename = argv[argc - 1];

	printf("Fetching %s\n", filename);

	return 0;
}
