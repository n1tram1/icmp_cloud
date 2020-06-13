#ifndef PING_H
#define PING_H

#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

int icmp_send(int sockfd, const struct sockaddr_in *dest, socklen_t destlen,
	      const void *payload, uint16_t payload_sz);
ssize_t icmp_recv(int sockfd, void *payload, uint16_t payload_sz, struct sockaddr_in *src_addr, socklen_t *addrlen);

#endif /* !PING_H */
