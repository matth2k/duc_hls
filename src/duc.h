#ifndef DUC_H

#define DUC_H
#include <stdint.h>

#define WRITE_REQ 0x0011
#define WRITE_RESP 0x0001
#define READ_REQ 0x0010
#define READ_RESP 0x0000

typedef uint32_t memt_t;
typedef uint32_t id_t;
typedef uint64_t addr_t;

#endif
