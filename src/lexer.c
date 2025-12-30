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

#define CUR (lxr->src_idx >= lxr->src_len ? '\0' : lxr->src[lxr->src_idx])

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

	while (alphanumeric(CUR)) {
		lxr->src_idx += 1;
	}

	uint32 len = lxr->src_idx - sym_start;

	token_tag tag;

	#define KEYWORD(keyword) strncmp(sym, keyword, strlen(keyword) > len ? strlen(keyword) : len) == 0

	if (KEYWORD("type")) {
		tag = TOK_TYPE;
	} else if (KEYWORD("never")) {
		tag = TOK_NEVER;
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
	while (whitespace(CUR)) {
		lxr->src_idx += 1;
	}
}

static void lex_comment(lexer *lxr) {
	assert(CUR == '#');

	while (CUR != '\n') {
		lxr->src_idx += 1;
	}

	lxr->src_idx += 1; // \n
}

static void lex_string(lexer *lxr) {
	lxr->src_idx += 1;

	while (CUR != '"') {
		lxr->src_idx += 1;
	}

	lxr->src_idx += 1;

	end_token(lxr, TOK_STRING);
}

static void lex_single(lexer *lxr, token_tag tag) {
	lxr->src_idx += 1;

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
	SINGLE('}', TOK_BRACKET_CLOSE);
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
		if (alphanumeric(lxr->src[lxr->src_idx + 1])) {
			lxr->src_idx += 1;
			while (alphanumeric(CUR)) {
				lxr->src_idx += 1;
			}
			end_token(lxr, TOK_ATOM);
			
		} else {
			lex_single(lxr, TOK_COLON);
		}
		break;


	case '"':
		lex_string(lxr);
		break;

	case '-':
		if (lxr->src[lxr->src_idx + 1] == '>') {
			lxr->src_idx += 2;
			end_token(lxr, TOK_ARROW);
		} else {
			fprintf(stderr, "expected '->'\n");
			exit(1);
		}
		break;

	default:
		if (numeric(CUR)) {
			while (alphanumeric(CUR)) {
				lxr->src_idx += 1;
			}

			end_token(lxr, TOK_NUMBER);
			break;
		}

		if (alphanumeric(CUR)) {
			lex_symbol(lxr);
			break;
		}

		fprintf(
			stderr,
			"error (lexer): unhandled character '%c' (%d)\n",
			CUR, lxr->src_idx
		);
		exit(1);
	}
}

token_list lex(char *src, int src_len) {
	lexer lxr;
	lxr.src = src;
	lxr.src_len = src_len;
	lxr.src_idx = 0;

	array_init(&lxr.toks, 64);

	while (lxr.src_idx < lxr.src_len) {
		printf("lexing (%d %c)\n", lxr.src_idx, lxr.src[lxr.src_idx]);
		lex_next(&lxr);
	}

	end_token(&lxr, TOK_EOF);

	return lxr.toks;
}

