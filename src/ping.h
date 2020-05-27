#ifndef PING_H
#define PING_H

#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <err.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>

#include "log.h"

#define ICMP_RESPONSE_MAX 96
#define PAYLOAD_MAX (ICMP_RESPONSE_MAX - sizeof(struct icmphdr))

struct ping_pkt {
	struct icmphdr hdr;
	uint8_t payload[PAYLOAD_MAX];
};


int ping_send(int sockfd, const struct sockaddr_in *dest, socklen_t len,
	      const void *payload);
int ping_recv(int sockfd);

#endif /* !PING_H */
