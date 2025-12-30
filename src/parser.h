#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

ARRAY_DECL(item_list, ast_item);

item_list parse(token_list toks);

#endif
