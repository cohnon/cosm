#ifndef LEXER_H
#define LEXER_H

#include "array.h"

typedef enum {
	TOK_INVALID,
	TOK_EOF,

	// values
	TOK_SYMBOL, // @name
	TOK_LABEL, // :label
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
	TOK_DOT,

	// keywords
	TOK_UNIT,
	TOK_NEVER,
	TOK_I8,
	TOK_I32,
	TOK_HALF,
	TOK_WORD,
	TOK_FN,
	TOK_TYPE,

	TOK_ADD_I32,
	TOK_CONST_I32,
	TOK_CALL,
	TOK_RETURN,
} TokenType;

typedef struct {
	TokenType type;
	char *src;
	size_t len;
} Token;

ARRAY_DECL(TokenArray, Token);

TokenArray lex(char *str, int len);
char *token_string(Token tok);
char *token_string_from_type(TokenType type);
#define TOK_FMT(tok) (int)(tok).len, (tok).src

#endif
