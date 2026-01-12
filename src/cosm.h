#ifndef COSM_H
#define COSM_H

#include <stdint.h>

typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int32_t int32;
typedef uint8_t uint8;

#define COSM_MAGIC ("cosm")
#define COSM_VERSION_MINOR (0)
#define COSM_VERSION_MAJOR (0)
#define COSM_VERSION ((COSM_VERSION_MAJOR << 16) & (COSM_VERSION_MINOR))

#endif
