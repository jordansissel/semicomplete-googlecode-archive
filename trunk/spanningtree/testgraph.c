#ifdef __FreeBSD__
#include <sys/time.h>
#include <sys/types.h>
#endif
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "graph.h"

int main(int argc, char **argv) {
	graph_t *g;
	int x;

	for (x = 0;1;x++) {
		if (x % 1000 == 0) 
			fprintf(stderr, "gen %d\n", x);
		g = gengraph(20, .5);
		freegraph(g);
	}

	return 0;
}
