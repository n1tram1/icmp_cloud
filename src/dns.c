#include "dns.h"

int dns_lookup(const char *hostname, struct sockaddr_in *res)
{
	struct hostent *host_entity;

	if ((host_entity = gethostbyname(hostname)) == NULL)
		return -1;

	res->sin_family = host_entity->h_addrtype;
	memcpy(&res->sin_addr, host_entity->h_addr, host_entity->h_length);

	LOG("dns_lookup: %s => %s\n", hostname,
	    inet_ntoa(*(struct in_addr *)host_entity->h_addr));
	return 0;
}
