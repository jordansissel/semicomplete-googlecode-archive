#include <string.h>

void fatal(char *msg) {
	perror(msg);
	exit(1);
}

