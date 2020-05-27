#include "juggler.h"

#define STACK_SIZE (1 << 16)

static int juggling_loop(void *p)
{
	// TODO: take the inconming chunks and start juggling them.
	const struct juggler *j = p;

	for (;;)

	return 0;
}

struct juggler *juggler_create(void)
{
	struct juggler *j = calloc(1, sizeof(*j));
	if (!j)
		return NULL;

	j->stack = malloc(STACK_SIZE);
	if (!j->stack)
		return NULL;


	j->pid = clone(juggling_loop, j->stack + STACK_SIZE, CLONE_VM, j);
	if (j->pid < 0)
		return NULL;


	return j;
}

void juggler_destroy(struct juggler **juggler)
{
	struct juggler *j = *juggler;

	free(j->stack);
	kill(j->pid, SIGKILL);

	free(j);

	*juggler = NULL;
}

int juggler_add_chunk(struct juggler *j, const struct chunk *chk)
{
	// TODO: actually push the chunk somewhere
	// TODO: maybe rename juggler_add_chunk -> juggler_transfer_chunk (because the ownership of the chunk is transfered to the juggler)
	printf("adding chunk %u (%u bytes): %.*s\n", chk->meta.idx, chk->meta.size, chk->meta.size, chk->data);
	return -1;
}
