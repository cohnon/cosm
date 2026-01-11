#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

ast_root parse(token_list toks);

#endif
