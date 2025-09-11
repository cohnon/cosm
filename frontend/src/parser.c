#include "parser.h"
#include "lexer.h"

#include "cosm.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    TokenArray toks;
    FILE *out;
} Parser;

static void write_header(Parser *psr) {
    CosmHeader hdr;
    memcpy(&hdr.magic, COSM_MAGIC, sizeof(hdr.magic));
    hdr.version = COSM_VERSION;
    hdr.size = 0;
    hdr.n_imports = 0;
    hdr.n_exports = 0;

    fwrite(&hdr, sizeof(hdr), 1, psr->out);
}

void parse(TokenArray toks, FILE *out) {
    Parser psr;
    psr.toks = toks;
    psr.out = out;

    write_header(&psr);
}
