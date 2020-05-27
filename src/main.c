#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <err.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"
#include "dns.h"
#include "ping.h"
#include "chunk.h"
#include "juggler.h"

#define array_len(arr) (sizeof(arr) / sizeof((arr)[0]))

static int ping(const char *hostname)
{
	int sockfd;
	struct sockaddr_in dest;

	memset(&dest, 0, sizeof(dest));

	if (dns_lookup(hostname, &dest) < 0) {
		warnx("DNS Lookup of %s failed", hostname);
		return -1;
	}

	// FIXME: fix for IPv6 compat (AF_INET & AF_INET6)
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		warnx("creation of socket for %s failed", hostname);
		return -1;
	}

	LOG("created socket %d for %s\n", sockfd, hostname);

	char payload[PAYLOAD_MAX] = "tamer";
	if (ping_send(sockfd, &dest, sizeof(dest), payload) < 0) {
		// FIXME: close the socket in case of error
		warnx("failed to send ping to %s", hostname);
		return -1;
	}

	if (ping_recv(sockfd))
		warnx("didn't receive ping reply from %s", hostname);

	if (close(sockfd) < 0) {
		warn("failed to close socket %d", sockfd);
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct juggler *juggler = NULL;

	if (argc < 2)
		errx(EXIT_FAILURE, "Usage: %s <hostname ...>", argv[0]);

	juggler = juggler_create();
	if (!juggler)
		errx(EXIT_FAILURE, "failed to create the juggler");

	for (int i = 1; i < argc; i++) {
		FILE *file = fopen(argv[i], "r");
		if (!file)
			errx(EXIT_FAILURE, "failed to open %s", argv[i]);

		uint32_t idx = 0;
		struct chunk *chk;
		while ((chk = chunkify(file, PAYLOAD_MAX)) != NULL) {
			chk->meta.idx = idx;

			juggler_add_chunk(juggler, chk);

			free(chk);
			idx++;
		}

		fclose(file);
	}

	juggler_destroy(&juggler);

	return 0;
}
