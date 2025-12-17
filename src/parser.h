#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct Module {
	ItemList items;
} Module;

Module *parse(TokenArray toks);

#endif
