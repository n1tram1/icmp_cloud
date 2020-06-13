#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <stddef.h>

uint64_t jenkins_one_at_a_time_hash(const char *key)
{
	size_t i = 0;
	uint64_t hash = 0;
	while (key[i]) {
		hash += key[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

#endif /* !HASH_H */
