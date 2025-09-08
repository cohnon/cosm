#include "fir.h"

#include <assert.h>
#include <stdio.h>

bool fir_type_eq(fir_DataType a, fir_DataType b) {
    return (a.kind == b.kind) && (a.data == b.data);
}

fir_DataType fir_type_void(void) {
    return fir_type_int(0);
}

fir_DataType fir_type_int(size_t data) {
    fir_DataType type;
    type.kind = FIR_TYPE_INT;
    type.data = data;

    return type;
}

fir_DataType fir_type_float(size_t data) {
    assert(data == 32 || data == 64);

    fir_DataType type;
    type.kind = FIR_TYPE_FLOAT;
    type.data = data;

    return type;
}

fir_DataType fir_type_ptr(void) {
    fir_DataType type;
    type.kind = FIR_TYPE_PTR;
    type.data = 64;

    return type;
}

fir_DataType fir_type_tuple(size_t n) {
    fir_DataType type;
    type.kind = FIR_TYPE_TUPLE;
    type.data = n;

    return type;
}

bool fir_type_is_void(fir_DataType type) {
    return type.kind == FIR_TYPE_INT && type.data == 0;
}

void fir_type_dump(fir_DataType type, FILE *fp) {
    switch (type.kind) {
    case FIR_TYPE_INT:
        fprintf(fp, "i%d", type.data);
        break;
    case FIR_TYPE_FLOAT:
        fprintf(fp, "f%d", type.data);
        break;
    case FIR_TYPE_PTR:
        fprintf(fp, "ptr");
        break;
    default:
        fprintf(fp, "<unhandled type>");
        break;
    }
}
