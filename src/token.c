#include "token.h"

char *token_string(token_tag tag) {
	switch (tag) {
	case TOK_INVALID: return "<invalid>";
	case TOK_EOF: return "<eof>";
	case TOK_SYMBOL: return "symbol";
	case TOK_ATOM: return ":atom";
	case TOK_NUMBER: return "<number>";
	case TOK_STRING: return "<string>";
	case TOK_CHARACTER: return "<character>";
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
	case TOK_DASH: return "-";
	case TOK_STAR: return "*";
	case TOK_SLASH: return "/";
	case TOK_BAR: return "|";
	case TOK_COLON: return ":";
	case TOK_SEMICOLON: return ";";
	case TOK_DOT: return ".";
	case TOK_NEVER: return "never";
	case TOK_TYPE: return "type";
	case TOK_FUNC: return "func";
	case TOK_VAR: return "var";
	case TOK_THROW: return "throw";
	case TOK_RETURN: return "return";
	default: return "<unhandled>";
	}
}
