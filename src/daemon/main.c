#include <err.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/dns.h"
#include "utils/ping.h"
#include "global/chunk_def.h"

static void loop(void)
{
	int sockfd;
	struct chunk chk;
	struct sockaddr_in src;
	socklen_t src_len = sizeof(src);

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		err(EXIT_FAILURE, "socket");

	while (1) {
		if (icmp_recv(sockfd, &chk, sizeof(chk), &src, &src_len) <= 0)
			continue;


		if (chk.meta.magic == CHUNK_MAGIC) {
			printf("payload from %s: {key: %lx, idx: %u size: %u}\n", inet_ntoa(src.sin_addr), chk.meta.key, chk.meta.idx, chk.meta.size);
			if (icmp_send(sockfd, &src, src_len, &chk, sizeof(chk)) < 0)
				warn("failed to re-send payload");
		}
	}
}

int main(int argc, char **argv)
{
	loop();

	return 0;
}
