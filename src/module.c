#include "module.h"

#include "lexer.h"
#include "lower.h"
#include "parser.h"
#include <stdio.h>

module module_init(char *src, int src_len) {
	module mod = {0};
	mod.src = src;
	mod.src_len = src_len;

	mod.alc = chunk_allocator_init(CHUNK_SIZE_SMALL);

	lex(&mod);

	printf("*** tokens ***\n");
	for (int i = 0; i < mod.toks.len; i += 1) {
		printf("%s ", token_string(mod.toks.items[i].tag));
	}
	printf("\n");

	parse(&mod);

	printf("*** ast ***\n");
	print_ast(&mod.ast);

	lower(&mod);

	return mod;
}
