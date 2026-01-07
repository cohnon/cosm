#ifndef TOKEN_H
#define TOKEN_H

#include "cosm.h"

typedef enum {
	TOK_INVALID,
	TOK_EOF,

	// values
	TOK_SYMBOL,
	TOK_ATOM,
	TOK_NUMBER,
	TOK_STRING,

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
	TOK_STAR,
	TOK_SLASH,
	TOK_BAR,
	TOK_COLON,
	TOK_SEMICOLON,
	TOK_DOT,

	// keywords
	TOK_LET,
	TOK_NEVER,
	TOK_TYPE,
	TOK_FN,

	TOK_THROW,
	TOK_RETURN,
} token_tag;

typedef struct {
	token_tag tag;
	uint32 start;
} token;

char *token_string(token_tag tag);

#endif
