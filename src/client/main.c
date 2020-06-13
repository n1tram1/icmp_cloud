#define _POSIX_C_SOURCE 2

#include <err.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "global/validate.h"
#include "global/chunk_def.h"
#include "juggler.h"

static int send_file(const char *filepath)
{
	int rv = 0;

	FILE *file = fopen(filepath, "r");
	if (!file) {
		warn("failed to open %s", filepath);
		return -1;
	}

	if (juggler_add_file(file, filepath) < 0)
		rv = -1;

	fclose(file);

	return rv;
}

static void show_usage(char **argv)
{
	warnx("Usage: %s [-h] [-g file]... [-s file]... [-l]", argv[0]);
}

int main(int argc, char **argv)
{
	/* We don't have static asserts so instead this... */
	validate_static_asserts();

	if (argc < 2) {
		show_usage(argv);
		exit(EXIT_FAILURE);
	}

	while (1) {
		int c = getopt(argc, argv, "hs:");
		if (c == -1)
			break;

		switch (c) {
			case 's':
				if (send_file(optarg) < 0)
					warn("couldn't send %s", optarg);
				break;
			case 'h':
			case '?':
				show_usage(argv);
				break;
		}
	}

	if (optind < argc)
		errx(EXIT_FAILURE, "invalid option \'%s\'", argv[optind]);

	return 0;
}
