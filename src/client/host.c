#include "host.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "utils/list.h"
#include "utils/dns.h"

struct host *host_create(const char *hostname)
{
	struct sockaddr_in sa_in;
	struct host *h = NULL;

	if (dns_lookup(hostname, &sa_in) < 0)
		return NULL;

	if ((h = calloc(1, sizeof(*h))) == NULL)
		return NULL;

	strncpy(h->name, hostname, sizeof(h->name) - 1);
	h->name[sizeof(h->name) - 1] = '\0';

	h->host_addr = sa_in;

	return h;
}

struct host *hosts_create(char **argv)
{
	struct host *head = calloc(1, sizeof(*head));;
	struct host *h;

	if (!head)
		return NULL;

	list_init(&head->list);

	for (; *argv; argv++) {
		h = host_create(*argv);
		if (!h) {
			warn("Can't use host %s, skipping it...", *argv);
			continue;
		}

		if (!head)
			head = h;
		else
			list_insert(head->list.prev, &h->list);
	}


	return head;
}

void hosts_destroy(struct host *hosts)
{
	struct host *cur;
	struct host *tmp;

	list_for_each_safe(cur, tmp, &hosts->list, list) {
		list_remove(&cur->list);
		free(cur);
	}

	free(hosts);
}
