#include "module.h"

#include "cosm.h"

void write_i32(int32_t n, FILE *out) {
    fwrite(&n, sizeof(n), 1, out);
}

void write_i64(int64_t n, FILE *out) {
    fwrite(&n, sizeof(n), 1, out);
}

void write_section(COSM_SecType type, ByteArray buf, FILE *out) {
    if (buf.len == 0) {
        return;
    }

    write_i32(type, out);
    write_i64(buf.len, out);
    fwrite(buf.items, buf.len, 1, out);
}

void write_header(FILE *out) {
    fwrite(COSM_MAGIC, sizeof(COSM_MAGIC), 1, out);
    write_i32(COSM_VERSION, out);
}

void write_module(Module *mod, FILE *out) {
    write_header(out);
    write_section(COSM_SEC_IMPORTS, mod->imports, out);
    write_section(COSM_SEC_EXPORTS, mod->exports, out);
    write_section(COSM_SEC_STRINGS, mod->strings, out);
    write_section(COSM_SEC_TYPES,   mod->types,   out);
    write_section(COSM_SEC_ITEMS,   mod->items,   out);
    write_section(COSM_SEC_CODE,    mod->code,    out);
    write_section(COSM_SEC_DATA,    mod->data,    out);
}
