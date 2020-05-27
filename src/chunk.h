#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"

struct chunk_meta {
    uint16_t key;
    uint32_t idx;
    uint16_t size;
};

struct chunk {
    struct chunk_meta meta;
    uint8_t data[0];
} __packed;

size_t chunk_size(const struct chunk *chk);
struct chunk *chunkify(FILE *in, uint16_t data_size);

#endif /* !CHUNK_H */
