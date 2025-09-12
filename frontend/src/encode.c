#include "encode.h"

#include <string.h>

static void write_i32(FILE *out, int32_t val) {
    fwrite(&val, sizeof(val), 1, out);
}

static void write_str(FILE *out, char *val, size_t len) {
    fwrite(val, 1, len, out);
}

static void write_header(FILE *out) {
    CosmHeader hdr;
    memcpy(&hdr.magic, COSM_MAGIC, sizeof(hdr.magic));
    hdr.version = COSM_VERSION;
    hdr.size = 0;
    hdr.imports_len = 0;
    hdr.exports_len = 0;

    write_str(out, hdr.magic, sizeof(hdr.magic));
    write_i32(out, hdr.version);
    write_i32(out, hdr.size);
    write_i32(out, hdr.imports_len);
    write_i32(out, hdr.exports_len);
}

void encode_module(Module *mod, FILE *out) {
  write_header(out);

  (void)mod;
}
