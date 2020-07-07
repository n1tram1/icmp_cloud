#ifndef SUBMITTER_H
#define SUBMITTER_H

#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
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
typedef __loff_t loff_t;
#include <liburing.h>
#include <sys/mman.h>

struct submitter {
  struct io_uring ring;
};

typedef int (completion_cb)(const struct io_uring_cqe *cqe);

int submitter_init(struct submitter *sub);

int submitter_send(struct submitter *sub, const struct sockaddr_in *dest,
                   const void *payload, uint16_t payload_sz);
int submitter_recv(struct submitter *sub, struct sockaddr_in *src,
                   socklen_t *srclen, void *payload, uint16_t payload_sz);

int submitter_get_completion(struct submitter *sub, completion_cb *cb);

#endif /* !SUBMITTER_H */
