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
	#define CASE(val, type) if (strncmp(keyword, val, strlen(val) > len ? strlen(val) : len) == 0) { \
		return type; \
	} else

	CASE("unit", TOK_UNIT)
	CASE("never", TOK_NEVER)
	CASE("i8", TOK_I8)
	CASE("i32", TOK_I32)
	CASE("half", TOK_HALF)
	CASE("word", TOK_WORD)
	CASE("fn", TOK_FN)
	CASE("type", TOK_TYPE)

	CASE("add.i32", TOK_ADD_I32)
	CASE("const.i32", TOK_CONST_I32)
	CASE("call", TOK_CALL)
	CASE("return", TOK_RETURN)

	return TOK_SYMBOL;
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

		case '{':
			lex_single(&lxr, TOK_BRACE_OPEN);
			break;

		case '}':
			lex_single(&lxr, TOK_BRACE_CLOSE);
			break;

		case ',':
			lex_single(&lxr, TOK_COMMA);
			break;

		case '*':
			lex_single(&lxr, TOK_STAR);
			break;

		case '/':
			lex_single(&lxr, TOK_SLASH);
			break;

		case '|':
			lex_single(&lxr, TOK_BAR);
			break;

		case ':':
			lex_single(&lxr, TOK_COLON);
			break;

		case '.':
			lex_single(&lxr, TOK_DOT);

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
			if (numeric(c)) {
				do {
					lxr.src_idx += 1;
					c = current_character(&lxr);
				} while (alphanumeric(c));

				new_token(&lxr, TOK_NUMBER);
				break;
			}

			if (alphanumeric(c)) {
				do {
					lxr.src_idx += 1;
					c = current_character(&lxr);
				} while (alphanumeric(c));

				TokenType type = get_keyword_type(lxr.cur_tok.src, current_token_length(&lxr));
				new_token(&lxr, type);
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

	new_token(&lxr, TOK_EOF);

	return lxr.toks;
}

char *token_string(Token tok) {
	return token_string_from_type(tok.type);
}

char *token_string_from_type(TokenType type) {
	switch (type) {
	case TOK_INVALID: return "<invalid>";
	case TOK_EOF: return "<eof>";
	case TOK_SYMBOL: return "sym";
	case TOK_NUMBER: return "<num>";
	case TOK_STRING: return "<str>";
	case TOK_LABEL: return ":label";
	case TOK_PAREN_OPEN: return "(";
	case TOK_PAREN_CLOSE: return ")";
	case TOK_BRACKET_OPEN: return "[";
	case TOK_BRACKET_CLOSE: return "]";
	case TOK_BRACE_OPEN: return "{";
	case TOK_BRACE_CLOSE: return "}";
	case TOK_COMMA: return ",";
	case TOK_ARROW: return "->";
	case TOK_EQUAL: return "=";
	case TOK_PLUS: return "+";
	case TOK_STAR: return "*";
	case TOK_SLASH: return "/";
	case TOK_BAR: return "|";
	case TOK_COLON: return ":";
	case TOK_DOT: return ".";
	case TOK_UNIT: return "unit";
	case TOK_NEVER: return "never";
	case TOK_I8: return "i8";
	case TOK_I32: return "i32";
	case TOK_HALF: return "half";
	case TOK_WORD: return "word";
	case TOK_FN: return "fn";
	case TOK_TYPE: return "type";
	case TOK_ADD_I32: return "add.i32";
	case TOK_CONST_I32: return "const.i32";
	case TOK_CALL: return "call";
	case TOK_RETURN: return "return";
	default: return "<unhandled>";
	}
}
