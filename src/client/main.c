#define _POSIX_C_SOURCE 2

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "global/validate.h"
#include "global/chunk_def.h"
#include "juggler.h"

static void show_usage(char **argv)
{
	warnx("Usage: %s [-h] [-g file]... [-s file]... [-l]", argv[0]);
}

static int send_file(const char *filepath, struct host *hosts)
{
	int rv = 0;

	FILE *file = fopen(filepath, "r");
	if (!file) {
		warn("failed to open %s", filepath);
		return -1;
	}

	if (juggler_add_file(file, filepath, hosts) < 0)
		rv = -1;

	fclose(file);

	return rv;
}

static int recv_file(const char *arg)
{
	uint64_t key;
	FILE *file;

	errno = 0;
	key = strtoul(arg, NULL, 0);
	if (errno)
		return -1;

	file = fopen(arg, "w");
	if (!file)
		return -1;

	if (juggler_get_file(file, key) < 0) {
		fclose(file);
		return -1;
	}

	fclose(file);

	return 0;
}

int main(int argc, char **argv)
{
	struct host *hosts = NULL;

	/* We don't have static asserts so instead this... */
	validate_static_asserts();

	if (argc < 2) {
		show_usage(argv);
		exit(EXIT_FAILURE);
	}

	hosts = hosts_create((char *[]){
		"epita.fr", "mirror.labkom.id", "mirror.poliwangi.ac.id",
		"suro.ubaya.ac.id", "mirror.rise.ph", NULL });

	while (1) {
		int c = getopt(argc, argv, "hs:g:");
		if (c == -1)
			break;

		switch (c) {
		case 's':
			if (send_file(optarg, hosts) < 0)
				warn("couldn't send %s", optarg);
			break;
		case 'g':
			if (recv_file(optarg) < 0)
				warn("couldn't recv %s", optarg);

			printf("received %s, saved to %s\n", optarg, optarg);

			break;
		case 'h':
		case '?':
			show_usage(argv);
			break;
		}
	}

	if (optind < argc)
		errx(EXIT_FAILURE, "invalid option \'%s\'", argv[optind]);

	hosts_destroy(hosts);

	return 0;
}
