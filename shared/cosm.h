#ifndef COSM_H
#define COSM_H

#include <stdint.h>

#define COSM_MAGIC ("cosm")

typedef uint64_t CosmWord;
typedef uint32_t CosmIdx;

enum CosmSecType {
    COSM_SEC_CUSTOM,
};
typedef uint32_t CosmSecType;

#endif
