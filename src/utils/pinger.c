#include "pinger.h"


#include <err.h>

#include "ping.h"

#define QUEUE_DEPTH 4

int submitter_init(struct submitter *sub)
{
	struct io_uring *ring = &sub->ring;
	int rv;

	rv = io_uring_queue_init(QUEUE_DEPTH, ring, 0);
	if (rv < 0) {
		warn("queue_init");
		return -1;
	}

	return 0;
}

int submitter_send(struct submitter *sub, const struct sockaddr_in *dest,
		   const void *payload, uint16_t payload_sz)
{
	int sockfd;

	struct icmphdr hdr = {
		.type = ICMP_ECHO,
		.code = 0,
		.un.echo = {
			.id = getpid(),
			.sequence = 0,
		},
	};

	struct msghdr msg = {
		.msg_name = (void *)dest,
		.msg_namelen = sizeof(*dest),
		.msg_iov =
			(struct iovec[]){
				{ .iov_base = &hdr, .iov_len = sizeof(hdr) },
				{ .iov_base = (void *)payload,
				  .iov_len = payload_sz },
			},
		.msg_iovlen = 2,
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_flags = 0,
	};

	hdr.checksum = icmp_iovec_checksum(msg.msg_iov, msg.msg_iovlen);

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		warn("socket");
		return -1;
	}

	struct io_uring_sqe *sqe = io_uring_get_sqe(&sub->ring);

	io_uring_prep_sendmsg(sqe, sockfd, &msg, 0);

	io_uring_sqe_set_data(sqe, (void *)0xCAFE);

	if (io_uring_submit(&sub->ring) != 1)
		err(1, "io_uring_submit");

	return 0;
}

int submitter_recv(struct submitter *sub, struct sockaddr_in *src,
		   socklen_t *srclen, void *payload, uint16_t payload_sz)
{
	(void)sub;
	(void)src;
	(void)srclen;
	(void)payload;
	(void)payload_sz;

	return -1;
}
