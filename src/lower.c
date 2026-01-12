#include "lower.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR(...) do { \
	fprintf(stderr, "lower error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

static void add_type(ast_type *type) {
	
}

static void lower_item(module *mod, ast_item *item) {
	(void)mod;
	switch (item->tag) {
	case ITEM_VARIABLE:
	}
}

void lower(module *mod) {
	mod->bc.types = buf_init(128);
	mod->bc.strings = buf_init(128);
	mod->bc.imports = buf_init(128);
	mod->bc.exports = buf_init(128);
	mod->bc.items = buf_init(128);
	mod->bc.code = buf_init(128);
	mod->bc.data = buf_init(128);

	for (int i = 0; i < mod->ast.items.len; i++) {
		lower_item(mod, mod->ast.items.items[i]);
	}
}
