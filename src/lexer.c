#include "lexer.h"
#include "array.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *src;
	int src_len;
	int src_idx;

	token cur_tok;
	token_list toks;
} lexer;

static void start_token(lexer *lxr) {
	lxr->cur_tok.start = lxr->src_idx;
}

static void end_token(lexer *lxr, token_tag tag) {
	lxr->cur_tok.tag = tag;

	array_append(&lxr->toks, &lxr->cur_tok);
}

#define SYNTAX_ERROR(...) do { \
	fprintf(stderr, "lex error: " __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	exit(EXIT_FAILURE); \
} while(0)

#define CUR (lxr->src_idx >= lxr->src_len ? '\0' : lxr->src[lxr->src_idx])
#define NEXT (lxr->src_idx - 1 >= lxr->src_len ? '\0' : lxr->src[lxr->src_idx + 1])

#define EAT lxr->src_idx += 1

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

static void lex_symbol(lexer *lxr) {
	uint32 sym_start = lxr->cur_tok.start;
	char *sym = &lxr->src[sym_start];

	while (alphanumeric(CUR)) { EAT; }

	uint32 len = lxr->src_idx - sym_start;

	token_tag tag;

	#define KEYWORD(keyword) strncmp(sym, keyword, strlen(keyword) > len ? strlen(keyword) : len) == 0

	if (KEYWORD("let")) {
		tag = TOK_LET;
	} else if (KEYWORD("type")) {
		tag = TOK_TYPE;
	} else if (KEYWORD("never")) {
		tag = TOK_NEVER;
	} else if (KEYWORD("fn")) {
		tag = TOK_FN;
	} else if (KEYWORD("foreign")) {
		tag = TOK_FOREIGN;
	} else if (KEYWORD("throw")) {
		tag = TOK_THROW;
	} else if (KEYWORD("return")) {
		tag = TOK_RETURN;
	} else {
		tag = TOK_SYMBOL;
	}

	end_token(lxr, tag);
}

static void skip_whitespace(lexer *lxr) {
	while (whitespace(CUR)) { EAT; }
}

static void lex_comment(lexer *lxr) {
	assert(CUR == '#');

	while (CUR != '\n') { EAT; }

	EAT; // \n
}

static void lex_string(lexer *lxr) {
	EAT; // "

	while (CUR != '"') { EAT; }

	EAT; // "

	end_token(lxr, TOK_STRING);
}

static void lex_single(lexer *lxr, token_tag tag) {
	EAT;

	end_token(lxr, tag);
}

static void lex_next(lexer *lxr) {
	skip_whitespace(lxr);

	start_token(lxr);

	#define SINGLE(c, t) case c: lex_single(lxr, t); break
	switch (CUR) {
	case '\0':
		break;

	SINGLE('+', TOK_PLUS);
	SINGLE('=', TOK_EQUAL);
	SINGLE('(', TOK_PAREN_OPEN);
	SINGLE(')', TOK_PAREN_CLOSE);
	SINGLE('[', TOK_BRACKET_OPEN);
	SINGLE(']', TOK_BRACKET_CLOSE);
	SINGLE('{', TOK_BRACE_OPEN);
	SINGLE('}', TOK_BRACE_CLOSE);
	SINGLE(',', TOK_COMMA);
	SINGLE('*', TOK_STAR);
	SINGLE('/', TOK_SLASH);
	SINGLE('|', TOK_BAR);
	SINGLE(';', TOK_SEMICOLON);
	SINGLE('.', TOK_DOT);

	case '#':
		lex_comment(lxr);
		break;


	case ':':
		if (alphanumeric(NEXT)) {
			EAT; // :
			while (alphanumeric(CUR)) { EAT; }
			end_token(lxr, TOK_ATOM);
			
		} else {
			lex_single(lxr, TOK_COLON);
		}
		break;


	case '"':
		lex_string(lxr);
		break;

	case '\'':
		EAT; // '
		// a character is either has the for '*' or '\*'
		// while a symbol is '*
		if (NEXT == '\'') {
			EAT; // *
			EAT; // '
			end_token(lxr, TOK_CHARACTER);
			break;
		}

		if (CUR == '\\') {
			EAT; // escape
			while (CUR != '\'') { EAT; }
			EAT; // '
			end_token(lxr, TOK_CHARACTER);
			break;
		}

		while (alphanumeric(CUR)) { EAT; }
		end_token(lxr, TOK_SYMBOL);
		break;

	case '-':
		if (NEXT == '>') {
			EAT; // -
			EAT; // >
			end_token(lxr, TOK_ARROW);
		} else {
			end_token(lxr, TOK_DASH);
		}
		break;

	default:
		if (numeric(CUR)) {
			while (alphanumeric(CUR)) { EAT; }
			end_token(lxr, TOK_NUMBER);
			break;
		}

		if (alphanumeric(CUR)) {
			lex_symbol(lxr);
			break;
		}

		SYNTAX_ERROR("unhandled character '%c' (%d)", CUR, lxr->src_idx);
	}
}

token_list lex(char *src, int src_len) {
	lexer lxr;
	lxr.src = src;
	lxr.src_len = src_len;
	lxr.src_idx = 0;

	array_init(&lxr.toks, 64);

	while (lxr.src_idx < lxr.src_len) {
		lex_next(&lxr);
	}

	end_token(&lxr, TOK_EOF);

	return lxr.toks;
}

