#include "io.h"
#include "module.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int src_len;
	char *src = io_file_read(argv[1], &src_len);

	module_init(src, src_len);

	return EXIT_SUCCESS;
}
