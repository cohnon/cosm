#include "cosm.h"
#include "encode.h"
#include <assert.h>
#include <stdint.h>

typedef struct Encoder {
    FILE   *out;
    Module *mod;
} Encoder;

void encode_u32(Encoder *enc, uint32_t val) {
    fwrite(&val, sizeof(val), 1, enc->out);
}

void encode_word(Encoder *enc, CosmWord ref) {
    fwrite(&ref, sizeof(ref), 1, enc->out);
}

void encode_idx(Encoder *enc, CosmIdx idx) {
    fwrite(&idx, sizeof(idx), 1, enc->out);
}

void write_str(Encoder *enc, char *val, size_t len) {
    fwrite(val, 1, len, enc->out);
}

void encode_bytes(Encoder *enc, unsigned char *bytes, size_t len) {
    fwrite(bytes, len, 1, enc->out);
}

void encode_func(Encoder *enc, Item *item) {
    encode_u32(enc, item->ty->idx);
}

void encode_imports(Encoder *enc, Import *import) {
    encode_word(enc, import->mod_name);
    encode_word(enc, import->item_name);
    encode_idx(enc, import->item_type);
}

void encode_exports(Encoder *enc, Export *export) {
    encode_word(enc, export->mod_name);
    encode_idx(enc, export->item_type);
}

void encode_functions(Encoder *enc, FunctionArray *funcs) {
    for (int i = 0; i < funcs->len; i++) {
        encode_word(enc, funcs->items[i].type);
    }
}

void encode_header(Encoder *enc) {
    Module *mod = enc->mod;

    unsigned char magic[] = COSM_MAGIC;
    encode_bytes(enc, magic, sizeof(magic));

    encode_u32(enc, 0x0); // version
    encode_word(enc, 0x0); // name (no name yet)

    encode_idx(enc, mod->imports.len);
    encode_idx(enc, mod->exports.len);
}

void encode(FILE *out, Module *mod) {
    Encoder enc;
    enc.out = out;
    enc.mod = mod;

    encode_header(&enc);

    for (int i = 0; i < mod->items.len; i++) {
        Item *item = mod->items.items[i];
        encode_item(&enc, item);
    }
}
