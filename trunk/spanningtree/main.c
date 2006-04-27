
/* Kruskal MST Algorithm
 *
 * Uses count-sort 
 *
 * $Id$
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "graph.h"

extern void do_algorithm(graph_t *);

int main(int argc, char **argv) {
	int v;
	float ep;
	graph_t *g;

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

	g = gengraph(v, ep);

	do_algorithm(g);

	return 0;
}
