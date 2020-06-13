#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>

#include "ping.h"

static unsigned short icmp_iovec_checksum(const struct iovec *iov, size_t iovcnt)
{
	unsigned int sum = 0;
	unsigned short result;

	for (size_t iov_i = 0; iov_i < iovcnt; iov_i++) {
		const struct iovec *cur_iov = iov + iov_i;
		const unsigned short *p = cur_iov->iov_base;
		int len = cur_iov->iov_len; /* int is ok because icmp payload can't exceed 65535 */

		for (; len > 1; len -= 2)
			sum += *p++;

		if (len == 1)
			sum += *(unsigned char *)p;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

// TODO: take a payload_size param
int icmp_send(int sockfd, const struct sockaddr_in *dest, socklen_t destlen,
	      const void *payload, uint16_t payload_sz)
{
	struct icmphdr hdr = {
		.type = ICMP_ECHO,
		.code = 0,
		.un.echo = {
			.id = getpid(),
			.sequence = 0,
		},
	};

	struct msghdr msg = {
		.msg_name = (void*)dest,
		.msg_namelen = destlen,
		.msg_iov = (struct iovec[]) {
				{ .iov_base = &hdr, .iov_len = sizeof(hdr) },
				{ 0 }, /* Reserved for the payload */
		},
		.msg_iovlen = 1, /* Start off assuming no payload */
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_flags = 0,
	};

	if (payload) {
		msg.msg_iov[1].iov_base = (void *)payload;
		msg.msg_iov[1].iov_len = payload_sz;

		msg.msg_iovlen = 2;
	}

	hdr.checksum = icmp_iovec_checksum(msg.msg_iov, msg.msg_iovlen);

	if (sendmsg(sockfd, &msg, 0) == -1) {
		warn("sendmsg ping failed");
		return -1;
	}

	return 0;
}

// TODO: take a payload_size param
ssize_t icmp_recv(int sockfd, void *payload, uint16_t payload_sz, struct sockaddr_in *src_addr, socklen_t *addrlen)
{
	uint8_t buf[IP_MAXPACKET];
	ssize_t bytes_read = 0;
	struct iphdr *ip_hdr = NULL;
	uint16_t ip_hdr_len = 0;

	if ((bytes_read = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)src_addr, addrlen)) < 0 )
		return -1;

	ip_hdr = (struct iphdr *)buf;
	ip_hdr_len = ip_hdr->ihl << 2;


	if (ip_hdr->protocol != IPPROTO_ICMP)
		return -1;

	uint16_t total_headers_len = ip_hdr_len + sizeof(struct icmphdr);

	if (bytes_read > total_headers_len) {
		uint16_t remaining_bytes = ip_hdr->tot_len - total_headers_len;
		if (remaining_bytes > payload_sz)
			remaining_bytes = payload_sz;

		if (payload) {
			memcpy(payload, buf + total_headers_len, remaining_bytes);
			return remaining_bytes;
		}
	}

	return 0;
}
