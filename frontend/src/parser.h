#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "module.h"

Module *parse(TokenArray toks);

#endif
