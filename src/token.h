#ifndef TOKEN_H
#define TOKEN_H

#include "array.h"
#include "cosm.h"

typedef enum {
	TOK_INVALID,
	TOK_EOF,

	// values
	TOK_SYMBOL,
	TOK_ATOM,
	TOK_NUMBER,
	TOK_STRING,
	TOK_CHARACTER,

	// symbols
	TOK_PAREN_OPEN,
	TOK_PAREN_CLOSE,
	TOK_BRACKET_OPEN,
	TOK_BRACKET_CLOSE,
	TOK_BRACE_OPEN,
	TOK_BRACE_CLOSE,
	TOK_COMMA,
	TOK_ARROW,
	TOK_EQUAL,
	TOK_PLUS,
	TOK_DASH,
	TOK_STAR,
	TOK_SLASH,
	TOK_BAR,
	TOK_COLON,
	TOK_SEMICOLON,
	TOK_DOT,

	// keywords
	TOK_NEVER,
	TOK_TYPE,
	TOK_FUNC,
	TOK_VAR,

	TOK_THROW,
	TOK_RETURN,
} token_tag;

typedef struct {
	token_tag tag;
	uint32 start;
} token;

ARRAY_DECL(token_list, token);

char *token_string(token_tag tag);

#endif
