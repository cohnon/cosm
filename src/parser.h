#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

ast_module parse(token_list toks);

#endif
