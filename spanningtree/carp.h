#include <string.h>

#define assertmsg(x, msg) do { \
	if (!x) fatal(msg); \
} while (0);

/* Fatal call for when malloc fails */
void fatal(char *msg);

