#include "lower.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR(...) do { \
	fprintf(stderr, "lower error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

static uint32_t add_string(module *mod, char *str, size_t len) {
	uint32_t ref = buf_push_varint(&mod->bc.strings, len);
	buf_push_bytes(&mod->bc.strings, str, len);

	return ref;
}

static uint32_t lower_type(module *mod, ast_type *type) {
	byte_buffer *types = &mod->bc.types;
	uint32_t ref = buf_ref(types);

	switch (type->tag) {
	case TYPE_UNIT:
		buf_push(types, TYPE_UNIT);
		break;

	case TYPE_FUNCTION:
		buf_push(types, TYPE_FUNCTION);
		lower_type(mod, type->fn.in);
		lower_type(mod, type->fn.out);
		break;

	default:
		ERROR("unhandled type %d", type->tag);
	}

	return ref;
}

static uint32_t lower_function(module *mod, ast_item *item) {
	uint32_t ref = buf_ref(&mod->bc.code);

	return ref;
}

static uint32_t lower_variable(module *mod, ast_item *item) {
	uint32_t ref;

	switch (item->var.type->tag) {
	case TYPE_FUNCTION:
		ref = lower_function(mod, item);
		break;

	default:
		ERROR("unhandled variable type %d", item->var.type->tag);
	}

	return ref;
}

static void lower_item(module *mod, ast_item *item) {
	switch (item->tag) {
	case ITEM_VARIABLE: {
		uint32_t type_ref = lower_type(mod, item->var.type);
		buf_push_varint(&mod->bc.items, type_ref);

		uint32_t storage_ref = lower_variable(mod, item);
		buf_push_varint(&mod->bc.items, storage_ref);
	}
	}
}

void lower(module *mod) {
	mod->bc.imports = buf_init(128);
	mod->bc.exports = buf_init(128);
	mod->bc.items = buf_init(128);

	mod->bc.code = buf_init(128);
	mod->bc.data = buf_init(128);
	mod->bc.types = buf_init(128);

	mod->bc.strings = buf_init(128);

	for (int i = 0; i < mod->ast.items.len; i++) {
		lower_item(mod, mod->ast.items.items[i]);
	}
}
