#include "chunk.h"

size_t chunk_size(const struct chunk *chk)
{
	return sizeof(*chk) + chk->meta.size;
}

struct chunk *chunkify(FILE *in, uint16_t max_chunk_size)
{
	const uint16_t data_size = max_chunk_size - sizeof(struct chunk);

	struct chunk *chk = calloc(1, sizeof(*chk) + data_size);
	if (!chk)
		return NULL;

	chk->meta.size = data_size;

	if ((fread(chk->data, 1, chk->meta.size, in)) == 0) {
		free(chk);
		return NULL;
	}

	return chk;
}
