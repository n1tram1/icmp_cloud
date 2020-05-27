#include "ping.h"

static unsigned short icmp_checksum(void *b, int len)
{
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

int ping_send(int sockfd, const struct sockaddr_in *dest, socklen_t destlen,
	      const void *payload)
{
	struct ping_pkt pkt = {
		.hdr = {
			.type = ICMP_ECHO,
			.code = 0,
			.un.echo = {
				.id = getpid(),
				.sequence = 0,
			},
		},
	};

	if (payload)
		memcpy(pkt.payload, payload, PAYLOAD_MAX);
	else
		memset(pkt.payload, 0, PAYLOAD_MAX);

	pkt.hdr.checksum = icmp_checksum(&pkt, sizeof(pkt));

	if (sendto(sockfd, &pkt, sizeof(pkt), 0, (const struct sockaddr *)dest,
		   destlen) == -1) {
		warnx("sendto ping failed");
		return -1;
	}

	LOG("sent ping\n");

	return 0;
}

int ping_recv(int sockfd)
{
	uint8_t buf[IP_MAXPACKET];
	struct iphdr *iph;
	struct ping_pkt *pkt;

	if (recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL) < 0)
		return -1;

	iph = (struct iphdr *)buf;
	pkt = (struct ping_pkt *)(buf + (iph->ihl << 2));

	if (pkt->hdr.type == ICMP_ECHOREPLY)
		LOG("received ping reply, payload: %.*s\n", (int)PAYLOAD_MAX,
		    pkt->payload);

	return 0;
}
