#define _POSIX_C_SOURCE
#define _GNU_SOURCE

#include "juggler.h"

#include <unistd.h>

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

	printf("sent chunk: {key: %lx, idx: %u size: %u}\n", chk->meta.key, chk->meta.idx, chk->meta.size);

	close(sockfd);

	return 0;
}

int juggler_add_file(FILE *file, const char *filename)
{
	struct chunk chk = {0};
	uint64_t hash = jenkins_one_at_a_time_hash(filename);

	uint32_t idx = 0;
	while (chunkify(file, &chk) > 0) {
		chk.meta.idx = idx;
		chk.meta.key = hash;

		if (juggler_add_chunk(&chk) < 0)
			return -1;

		idx++;
	}

	return 0;
}
