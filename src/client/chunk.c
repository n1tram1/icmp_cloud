#include "chunk.h"

size_t chunk_size(const struct chunk *chk)
{
	return sizeof(*chk) + chk->meta.size;
}

ssize_t chunkify(FILE *in, struct chunk *chk)
{
	ssize_t bytes_read;

	chk->meta.magic = CHUNK_MAGIC;
	chk->meta.key = 0;
	chk->meta.idx = 0;
	chk->meta.file_size = 0;

	if ((bytes_read = fread(chk->data, 1, sizeof(chk->data), in)) == 0) {
		return -1;
	}

	chk->meta.size = bytes_read;

	return bytes_read;
}
