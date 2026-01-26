#include "lower.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR(...) do { \
	fprintf(stderr, "lower error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

uint64 lower_type(module *mod, ast_type *type) {
	return 0;
}

uint64 lower_function_signature(module *mod, ast_item *item) {
	return 0;
}

uint64 lower_function_body(module *mod, ast_item *item) {
	return 0;
}

void lower_item(module *mod, ast_item *item) {
	switch (item->tag) {
	case ITEM_FUNCTION: {
		uint64 type_ref = lower_function_signature(mod, item);
		if (item->func.body != NULL) {
			uint64 code_ref = lower_function_body(mod, item);
			buf_push_varint(&mod->bc.funcs, type_ref);
			buf_push_varint(&mod->bc.funcs, code_ref);
		}
		break;
	}

	case ITEM_VARIABLE:
		break;
	}
}

void lower(module *mod) {
	mod->bc.imports = buf_init(128);
	mod->bc.exports = buf_init(128);
	mod->bc.exports_extra = buf_init(128);

	mod->bc.funcs = buf_init(128);
	mod->bc.vars = buf_init(128);
	mod->bc.types = buf_init(128);

	mod->bc.code = buf_init(128);
	mod->bc.data = buf_init(128);

	for (int i = 0; i < mod->ast.items.len; i++) {
		// lower item
	}
}
