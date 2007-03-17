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

	int v;
	float ep;

	if (argc < 3) {
		fprintf(stderr, "Invalid number of arguments. Usage: %s numvertex edgeprob\n", *argv);
		return 1;
	}

	if (sscanf(*++argv, "%d", &v) == 0) {
		fprintf(stderr,"First argument is bad.\n");
		return 1;
	}

	if (sscanf(*++argv, "%f", &ep) == 0) {
		fprintf(stderr,"Second argument is bad.\n");
		return 1;
	}

	if (argc == 4)
		srand(atoi(*++argv));
	else
		srand(time(NULL));

	for (x = 0;1;x++) {
		int i, j;
		g = gengraph(v, ep);
		
		for (i = 0; i < v; i++) {
			for (j = 0; j < v; j++) {
				int a = WEIGHT(g, i, j);
			}
		}

		if (x % 1000 == 0) {
			fprintf(stderr, "gen %d\n", x);
			fprintf(stderr, "%d->%d: %d\n", v / 3, v / 8, WEIGHT(g, v / 3, v / 8));
		}

		freegraph(g);
	}

	return 0;
}

