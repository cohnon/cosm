#ifndef LEXER_H
#define LEXER_H

#include "array.h"

typedef enum {
    TOK_INVALID,

    // values
    TOK_SYMBOL,   // @name
    TOK_VARIABLE, // %1
    TOK_NUMBER,
    TOK_STRING,

    // symbols
    TOK_PAREN_OPEN,
    TOK_PAREN_CLOSE,
    TOK_BRACKET_OPEN,
    TOK_BRACKET_CLOSE,
    TOK_COMMA,
    TOK_ARROW,
    TOK_EQUAL,
    TOK_PLUS,

    // keywords
    TOK_UNIT,
    TOK_NEVER,
    TOK_I32,
    TOK_HALF,
    TOK_WORD,
    TOK_PTR,

    TOK_ADD,
    TOK_CALL,
    TOK_RET,

    TOK_ENTRY_POINT,
} TokenType;

typedef struct {
    TokenType type;
    char     *src;
    size_t    len;
} Token;

ARRAY_DECL(TokenArray, Token);

TokenArray lex(char *str, int len);
char *token_string(Token tok);

#endif
