#ifndef JUGGLER_H
#define JUGGLER_H

#include <stdio.h>
#include <stdint.h>

int juggler_add_file(FILE *file, const char *filename);

int juggler_get_file(FILE *out, uint64_t key);

#endif /* !JUGGLER_H */
