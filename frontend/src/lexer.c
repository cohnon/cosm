#include "lexer.h"
#include "array.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Lexer {
    char *src;
    int src_len;
    int src_idx;

    Token cur_tok;
    TokenArray toks;
} Lexer;

static size_t current_token_length(Lexer *lxr) {
    return &lxr->src[lxr->src_idx] - lxr->cur_tok.src;
}

static void new_token(Lexer *lxr, TokenType type) {
    lxr->cur_tok.type = type;
    lxr->cur_tok.len = current_token_length(lxr);

    array_append(&lxr->toks, &lxr->cur_tok);
}

static char current_character(Lexer *lxr) {
    if (lxr->src_idx >= lxr->src_len) {
        return '\0';
    }

    return lxr->src[lxr->src_idx];
}

static int whitespace(char c) {
    return c == ' '  || c == '\t' || c == '\r' || c == '\n';
}

static int alpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static int numeric(char c) {
    return '0' <= c && c <= '9';
}

static int alphanumeric(char c) {
    return alpha(c) || numeric(c) || c == '_' || c == '.';
}

static TokenType get_keyword_type(char *keyword, size_t len) {
    #define CASE(val, type) if (strncmp(keyword, val, len) == 0) { \
        return type; \
    } else

    CASE("never", TOK_NEVER)
    CASE("unit", TOK_UNIT)
    CASE("i32", TOK_I32)
    CASE("half", TOK_HALF)
    CASE("word", TOK_WORD)
    CASE("ptr", TOK_PTR)
    CASE("add", TOK_ADD)
    CASE("call", TOK_CALL)
    CASE("ret", TOK_RET)
    CASE("entry_point", TOK_ENTRY_POINT)

    return TOK_INVALID;
}

static void skip_whitespace(Lexer *lxr) {
    while (whitespace(current_character(lxr))) {
        lxr->src_idx += 1;
    }
}

static void lex_comment(Lexer *lxr) {
    assert(current_character(lxr) == '#');

    while (current_character(lxr) != '\n') {
        lxr->src_idx += 1;
    }

    lxr->src_idx += 1; // \n
}

static void lex_symbol(Lexer *lxr) {
    while (!whitespace(current_character(lxr))) {
        lxr->src_idx += 1;
    }

    new_token(lxr, TOK_SYMBOL);
}

static void lex_variable(Lexer *lxr) {
    while (!whitespace(current_character(lxr))) {
        lxr->src_idx += 1;
    }

    new_token(lxr, TOK_VARIABLE);
}

static void lex_string(Lexer *lxr) {
    lxr->src_idx += 1;

    while (current_character(lxr) != '"') {
        lxr->src_idx += 1;
    }

    lxr->src_idx += 1;

    new_token(lxr, TOK_STRING);
}

static void lex_single(Lexer *lxr, TokenType type) {
    lxr->src_idx += 1;

    new_token(lxr, type);
}

TokenArray lex(char *src, int src_len) {
    Lexer lxr;
    lxr.src = src;
    lxr.src_len = src_len;
    lxr.src_idx = 0;

    array_init(&lxr.toks, 32);

    while (lxr.src_idx < lxr.src_len) {
        skip_whitespace(&lxr);

        lxr.cur_tok.src = &lxr.src[lxr.src_idx];

        char c = current_character(&lxr);
        switch (c) {
            case '\0':
                break;

            case '#':
                lex_comment(&lxr);
                break;

            case '+':
                lex_single(&lxr, TOK_PLUS);
                break;

            case '=':
                lex_single(&lxr, TOK_EQUAL);
                break;

            case '(':
                lex_single(&lxr, TOK_PAREN_OPEN);
                break;

            case ')':
                lex_single(&lxr, TOK_PAREN_CLOSE);
                break;

            case '[':
                lex_single(&lxr, TOK_BRACKET_OPEN);
                break;

            case ']':
                lex_single(&lxr, TOK_BRACKET_CLOSE);
                break;

            case ',':
                lex_single(&lxr, TOK_COMMA);
                break;

            case '@':
                lex_symbol(&lxr);
                break;

            case '%':
                lex_variable(&lxr);
                break;

            case '"':
                lex_string(&lxr);
                break;

            case '-':
                if (lxr.src[lxr.src_idx + 1] == '>') {
                    lxr.src_idx += 2;
                    new_token(&lxr, TOK_ARROW);
                } else {
                    fprintf(stderr, "expected '->'\n");
                    exit(1);
                }
                break;

            default:
                if (alpha(c)) {
                    do {
                        lxr.src_idx += 1;
                        c = current_character(&lxr);
                    } while (alphanumeric(c));
            
                    TokenType type = get_keyword_type(lxr.cur_tok.src, current_token_length(&lxr));
                    new_token(&lxr, type);
                    break;
                }

                if (numeric(c)) {
                    do {
                        lxr.src_idx += 1;
                        c = current_character(&lxr);
                    } while (alphanumeric(c));

                    new_token(&lxr, TOK_NUMBER);
                    break;
                }

                fprintf(
                    stderr,
                    "error (lexer): unhandled character '%c' (%d)\n",
                    c, lxr.src_idx
                );
                exit(1);
        }
    }

    return lxr.toks;
}

char *token_string(Token tok) {
    switch (tok.type) {
        case TOK_INVALID: return "<invalid>";
        case TOK_SYMBOL: return "@sym";
        case TOK_NUMBER: return "<num>";
        case TOK_STRING: return "<str>";
        case TOK_VARIABLE: return "%var";
        case TOK_PAREN_OPEN: return "(";
        case TOK_PAREN_CLOSE: return ")";
        case TOK_BRACKET_OPEN: return "[";
        case TOK_BRACKET_CLOSE: return "]";
        case TOK_COMMA: return ",";
        case TOK_ARROW: return "->";
        case TOK_EQUAL: return "=";
        case TOK_PLUS: return "+";
        case TOK_NEVER: return "never";
        case TOK_UNIT: return "unit";
        case TOK_I32: return "i32";
        case TOK_HALF: return "half";
        case TOK_WORD: return "word";
        case TOK_PTR: return "ptr";
        case TOK_ADD: return "add";
        case TOK_CALL: return "call";
        case TOK_RET: return "ret";
        case TOK_ENTRY_POINT: return "entry_point";
    }
}
