#ifndef HOST_H
#define HOST_H

#include <sys/param.h>
#include <arpa/inet.h>

#include "utils/list.h"

struct host {
  char name[MAXHOSTNAMELEN + 1];
  struct sockaddr_in host_addr;
  struct list list;
};

struct host *hosts_create(char **argv);

void hosts_destroy(struct host *hosts);

#endif /* !HOST_H */
