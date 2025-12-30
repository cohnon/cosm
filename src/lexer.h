#ifndef LEXER_H
#define LEXER_H

#include "array.h"
#include "token.h"

ARRAY_DECL(token_list, token);

token_list lex(char *src, int src_len);

#endif
