#ifndef JUGGLER_H
#define JUGGLER_H

#define _POSIX_C_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <signal.h>

#include "chunk.h"

struct juggler {
  pid_t pid;
  uint8_t *stack;
};

struct juggler *juggler_create(void);
void juggler_destroy(struct juggler **juggler);

int juggler_add_chunk(struct juggler *j, const struct chunk *chk);

#endif /* !JUGGLER_H */
