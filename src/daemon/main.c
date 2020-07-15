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

#include "utils/submitter.h"

#include "utils/dns.h"
#include "utils/ping.h"
#include "global/chunk_def.h"

static void loop(struct submitter *sub)
{
	struct chunk chk;

	struct sockaddr_in src;

	while (1) {
		int ret = submitter_recv(sub, &src, &chk, sizeof(chk));

		if (ret < 0)
			err(EXIT_FAILURE, "submitter_recv failed");

		if (!ret)
			continue;

		if (chk.meta.magic == CHUNK_MAGIC) {
			/* printf("payload from %s: {key: %lx, idx: %u size: %u}\n", */
			/*        inet_ntoa(src.sin_addr), chk.meta.key, */
			/*        chk.meta.idx, chk.meta.size); */

			submitter_send(sub, &src, &chk, sizeof(chk));
		}

	}
}

int main(int argc, char **argv)
{
	int sockfd;
	struct submitter sender = { 0 };

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		err(EXIT_FAILURE, "socket");

	if (submitter_init(&sender, sockfd) < 0) {
		warnx("setup of io_uring failed");
		exit(EXIT_FAILURE);
	}

	loop(&sender);

	return 0;
}
