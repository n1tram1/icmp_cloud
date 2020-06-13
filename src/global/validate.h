#ifndef VALIDATE_H
#define VALIDATE_H

#include <assert.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "chunk_def.h"

static inline void validate_static_asserts(void)
{
	assert(sizeof(struct chunk) == ICMP_PAYLOAD_MAX);
}

#endif /* !VALIDATE_H */
