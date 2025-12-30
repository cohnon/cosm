#include "io.h"
#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int src_len;
    char *src = io_file_read(argv[1], &src_len);

    token_list toks = lex(src, src_len);

    fprintf(stderr, "*** tokens ***\n");
    for (int i = 0; i < toks.len; i += 1) {
        fprintf(stderr, "%s ", token_string(toks.items[i].tag));
    }
    fprintf(stderr, "\n");

    parse(toks);
}
