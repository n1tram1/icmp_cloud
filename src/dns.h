#ifndef DNS_H
#define DNS_H

#define h_addr h_addr_list[0]

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

#include "log.h"

int dns_lookup(const char *hostname, struct sockaddr_in *res);

#endif /* !DNS_H */
