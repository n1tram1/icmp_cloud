#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#define __USE_MISC
#include <sys/types.h>
#include <sys/ucontext.h>
#include <liburing.h>
#include <sys/mman.h>

#include "utils/pinger.h"

#include "utils/dns.h"
#include "utils/ping.h"
#include "global/chunk_def.h"

#define QD 4

int get_completion_and_print(struct io_uring *ring) {
	struct io_uring_cqe *cqe;
	int ret = io_uring_wait_cqe(ring, &cqe);
	if (ret < 0) {
		perror("io_uring_wait_cqe");
		return 1;
	}
	if (cqe->res < 0) {
		fprintf(stderr, "Async readv failed.\n");
		return 1;
	}

	printf("received cqe (data: %p)\n", io_uring_cqe_get_data(cqe));

	io_uring_cqe_seen(ring, cqe);
	return 0;
}

static void loop(struct submitter *sub)
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
			printf("payload from %s: {key: %lx, idx: %u size: %u}\n",
			       inet_ntoa(src.sin_addr), chk.meta.key,
			       chk.meta.idx, chk.meta.size);

			submitter_send(sub, &src, &chk, sizeof(chk));
			get_completion_and_print(&sub->ring);
		}
	}
}

int main(int argc, char **argv)
{
	struct submitter sub = { 0 };

	if (submitter_init(&sub) < 0) {
		warnx("setup of io_uring failed");
		exit(EXIT_FAILURE);
	}

	loop(&sub);

	return 0;
}
