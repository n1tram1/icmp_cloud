#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "global/chunk_def.h"

size_t chunk_size(const struct chunk *chk);
ssize_t chunkify(FILE *in, struct chunk *chk);

#endif /* !CHUNK_H */
