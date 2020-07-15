#include "submitter.h"


#include <err.h>

#include "ping.h"

#define QUEUE_DEPTH (1U << 10)

int submitter_init(struct submitter *sub, int fd)
{
	struct io_uring_params params = { 0 };
	int rv;

	sub->fds[0] = fd;

	params.flags |= IORING_SETUP_SQPOLL;
	params.sq_thread_idle = 2000;

	rv = io_uring_queue_init_params(QUEUE_DEPTH, &sub->ring, &params);
	if (rv < 0) {
		warn("queue_init of depth %u", QUEUE_DEPTH);
		return -1;
	}

	if (io_uring_register_files(&sub->ring, sub->fds, 1)) {
		warn("register_files of fd: %d", sub->fds[0]);
		return -1;
	}

	return 0;
}

void print_sq_poll_kernel_thread_status() {

	if (system("ps --ppid 2 | grep io_uring-sq" ) == 0)
		printf("Kernel thread io_uring-sq found running...\n");
	else
		printf("Kernel thread io_uring-sq is not running.\n");
}

int submitter_send(struct submitter *sub, const struct sockaddr_in *dest,
		   const void *payload, uint16_t payload_sz)
{
	struct icmphdr hdr = {
		.type = ICMP_ECHO,
		.code = 0,
		.un.echo = {
			.id = 0x42,
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

	struct io_uring_cqe *cqe;

	hdr.checksum = icmp_iovec_checksum(msg.msg_iov, msg.msg_iovlen);

	struct io_uring_sqe *sqe = io_uring_get_sqe(&sub->ring);

	io_uring_prep_sendmsg(sqe, 0, &msg, 0);
	sqe->flags |= IOSQE_FIXED_FILE;

	print_sq_poll_kernel_thread_status();

	if (io_uring_submit(&sub->ring) == -1) {
		warn("io_uring_submit");
		return -1;
	}

	if (io_uring_wait_cqe(&sub->ring, &cqe) == -1) {
		warn("io_uring_wait_cqe");
		return -1;
	}

	io_uring_cqe_seen(&sub->ring, cqe);

	return 0;
}

int submitter_recv(struct submitter *sub, struct sockaddr_in *src,
		   void *payload, uint16_t payload_sz)
{
	int ret = -1;
	struct iphdr ip_hdr;
	struct icmphdr icmp_hdr;

	struct msghdr msg = {
		.msg_name = src,
		.msg_namelen = sizeof(*src),
		.msg_iov = (struct iovec[]) {
			{ .iov_base = &ip_hdr, .iov_len = sizeof(ip_hdr) },
			{ .iov_base = &icmp_hdr, .iov_len = sizeof(icmp_hdr) },
			{ .iov_base = payload, .iov_len = payload_sz },
		},
		.msg_iovlen = 3,
		.msg_control = 0,
		.msg_controllen = 0,
		.msg_flags = 0,
	};

	struct io_uring_sqe *sqe = io_uring_get_sqe(&sub->ring);
	struct io_uring_cqe *cqe;

	ssize_t bytes_received;

	io_uring_prep_recvmsg(sqe, 0, &msg, 0);
	sqe->flags |= IOSQE_FIXED_FILE;

	io_uring_sqe_set_data(sqe, (void*)0xCACA);

	if (io_uring_submit(&sub->ring) == -1) {
		warn("io_uring_submit");
		return -1;
	}

	if (io_uring_wait_cqe(&sub->ring, &cqe) < 0) {
		warn("io_uring_wait_cqe");
		return -1;
	}

	bytes_received = cqe->res;

	if (bytes_received < 0) {
		warn("async recvmsg failed");
		return -1;
	}

	/* We assume we get IP packets without the options filled (IHL == 5). */
	if (ip_hdr.ihl != 5) {
		ret = 0;
		goto out;
	}

	if (bytes_received > sizeof(ip_hdr) + sizeof(icmp_hdr))
		ret = 1;

out:
	io_uring_cqe_seen(&sub->ring, cqe);

	return ret;
}
