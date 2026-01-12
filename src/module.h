#ifndef MODULE_H
#define MODULE_H

#include "bytecode.h"
#include "token.h"
#include "ast.h"
#include "chunk_allocator.h"

typedef struct {
	char *src;
	int src_len;

	token_list toks;
	int toks_idx; // current token being parsed

	ast_root ast;

	bytecode bc;

	chunk_allocator alc;
} module;

module module_init(char *src, int src_len);

#endif
