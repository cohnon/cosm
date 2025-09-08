#include "fir.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>

static fir_StringID find_string(
    fir_Module *module,
    const char *str,
    size_t len
) {
    for (size_t i = 0; i < module->string_table.len; i++) {
        char *existing = *fir_array_get(char *, &module->string_table, i);
        if (strncmp(str, existing, len) == 0) {
            return i;
        }
    }

    return FIR_STRING_INVALID;
}

// creates a string with an id stored after '\0'
static fir_StringID create_string(
    fir_Module *module,
    const char *str,
    size_t len
) {
    char *new = fir_arena_alloc(&module->arena, len + 2);

    strcpy(new, str);

    fir_array_append(char *, &module->string_table, new);

    return module->string_table.len - 1;
}

fir_StringID fir_string_add(fir_Module *module, const char *c_str) {
    assert(module != NULL);
    assert(c_str != NULL && c_str[0] != 0);

    size_t len = strlen(c_str);

    fir_StringID found = find_string(module, c_str, len);

    if (found != FIR_STRING_INVALID) {
        return found;
    }

    return create_string(module, c_str, len);
}

char *fir_string_get(fir_Module *module, fir_StringID string) {
    return *fir_array_get(char *, &module->string_table, string);
}
