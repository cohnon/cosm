#include "io.h"

#include <stdio.h>
#include <stdlib.h>

char *io_file_read(char *path, int *src_len) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "error: could not open %s\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    int len = (int)ftell(f);
    fseek(f, 0, SEEK_SET);

    char *src = malloc(len);
    *src_len = len;
    int read = (int)fread(src, 1, len, f);

    fclose(f);

    if (read != len) {
        fprintf(
            stderr,
            "error: could not read file. read %d. expected %d\n",
            read, len
        );
    }

    return src;
}
