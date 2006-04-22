/* 
 * graph.c - jls
 *
 * $Id$
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "graph.h"

/* Initialize the graph adjacency matrix */
static void initgraph(graph_t *g) { 
	int x;
	g->matrix = malloc(g->numvert * sizeof(int));
	for (x = 0; x < g->numvert; x++) {
		*(g->matrix + x) = malloc(g->numvert * sizeof(int));
		memset(*(g->matrix), 0, g->numvert * sizeof(int));
	}
}

static void randomize(graph_t *g) {
	srand(time(NULL));
}

static int connected(graph_t *g) {
	return 0;
}

/***
 * Public functions 
 ***/

graph_t* gengraph(int v, float ep) {
	graph_t *g;

	assert(v > 0);
	assert(ep > 0 && ep < 1);

	g = malloc(sizeof(graph_t));
	memset(g, 0, sizeof(graph_t));

	g->numvert = v;
	g->edgeprob = ep;

	/* Initialize adjacency matrix */
	initgraph(g);
	do {
		/* generate random graph */
		randomize(g);
		/* ensure it is connected */
	} while (!connected(g));
}

#ifdef TEST
int main(int argc, char **argv) {
	int v;
	float ep;

	if (sscanf(*++argv, "%d", &v) == 0) {
		fprintf(stderr,"First argument is bad.\n");
		exit(1);
	}

	if (sscanf(*++argv, "%f", &ep)) {
		fprintf(stderr,"Second argument is bad.\n");
		exit(1);
	}

	printf("V: %d\nEP: %f\n", v, ep);

	return 0;
}
#endif /* TEST */
