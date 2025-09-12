#include "parser.h"
#include "lexer.h"

typedef struct {
    TokenArray toks;
    CosmModule mod;
} Parser;


CosmModule parse(TokenArray toks) {
    Parser psr;
    psr.toks = toks;
    psr.mod = (Module){0};

    return psr.mod;
}
