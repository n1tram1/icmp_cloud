#define _POSIX_C_SOURCE
#define _GNU_SOURCE

#include "juggler.h"

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#include "global/chunk_def.h"
#include "utils/dns.h"
#include "utils/ping.h"
#include "global/hash.h"
#include "chunk.h"

static int juggler_add_chunk(const struct chunk *chk)
{
	struct sockaddr_in dest;
	int sockfd;

	if (dns_lookup("epita.fr"/*"mirror.xeonbd.com"*/, &dest) < 0)
		return -1;

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
		return -1;

	if (icmp_send(sockfd, &dest, sizeof(dest), chk, sizeof(*chk)) < 0) {
		close(sockfd);
		return -1;
	}

	printf("sent chunk: {key: %lx, idx: %u size: %u, total_size: %lu}\n", chk->meta.key, chk->meta.idx, chk->meta.size, chk->meta.file_size);

	close(sockfd);

	return 0;
}

static size_t filesize(FILE *file)
{
	int fd = fileno(file);
	struct stat statbuf;

	if (fstat(fd, &statbuf) < 0)
		return -1;

	return statbuf.st_size;
}

int juggler_add_file(FILE *file, const char *filename)
{
	struct chunk chk = {0};
	uint64_t hash = jenkins_one_at_a_time_hash(filename);
	size_t file_size = filesize(file);

	uint32_t idx = 0;
	while (chunkify(file, &chk) > 0) {
		chk.meta.idx = idx;
		chk.meta.key = hash;
		chk.meta.file_size = file_size;

		if (juggler_add_chunk(&chk) < 0)
			return -1;

		idx++;
	}

	return 0;
}

int juggler_get_file(FILE *file, uint64_t key)
{
	int sockfd;
	struct chunk chk;

	uint32_t cur_chunk_idx = 0;
	uint32_t last_chunk_idx = 0;


	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
		return -1;

	while (cur_chunk_idx <= last_chunk_idx) {
		if (icmp_recv(sockfd, &chk, sizeof(chk), NULL, 0) <= 0)
			continue;

		if (chk.meta.magic != CHUNK_MAGIC || chk.meta.key != key || chk.meta.idx != cur_chunk_idx)
			continue;

		if (!last_chunk_idx)
			last_chunk_idx = chk.meta.file_size / CHUNK_DATA_MAX;

		fwrite(chk.data, chk.meta.size, sizeof(char), file);

		cur_chunk_idx++;
	}

	return 0;
}
