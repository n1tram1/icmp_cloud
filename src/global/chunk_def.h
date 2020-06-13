#ifndef CHUNK_DEF_H
#define CHUNK_DEF_H

#include <netinet/ip_icmp.h>
#include <stddef.h>

#include "compiler.h"

#define ICMP_RESPONSE_MAX 96
#define ICMP_PAYLOAD_MAX (ICMP_RESPONSE_MAX - sizeof(struct icmphdr))

#define CHUNK_MAGIC 0xCAFECACACAFEBABE

struct chunk_meta {
    uint64_t magic;
    uint64_t key;
    uint32_t idx;
    uint16_t size;
    uint64_t file_size;
} __packed;

#define CHUNK_DATA_MAX ((ICMP_PAYLOAD_MAX) - sizeof(struct chunk_meta))

struct chunk {
    struct chunk_meta meta;
    uint8_t data[CHUNK_DATA_MAX];
} __packed;

#endif /* !CHUNK_DEF_H */
