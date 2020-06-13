/* #include <netinet/ip_icmp.h> */
/* #include <stdio.h> */
/* #include <sys/types.h> */
/* #include <sys/socket.h> */
/* #include <err.h> */
/* #include <unistd.h> */
/* #include <netdb.h> */
/* #include <stdlib.h> */
/* #include <string.h> */
/* #include <sys/socket.h> */
/* #include <netinet/in.h> */
/* #include <arpa/inet.h> */
#include <err.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "global/validate.h"
#include "global/chunk_def.h"

int main(int argc, char **argv)
{
	/* We don't have static asserts so instead this... */
	validate_static_asserts();

	if (argc < 2)
		errx(EXIT_FAILURE, "Usage: %s <file ...>", argv[0]);

	for (int i = 1; i < argc; i++) {
		FILE *file = fopen(argv[i], "r");
		if (!file)
			errx(EXIT_FAILURE, "failed to open %s", argv[i]);

		juggler_add_file(file, argv[i]);

		fclose(file);
	}

	return 0;
}
