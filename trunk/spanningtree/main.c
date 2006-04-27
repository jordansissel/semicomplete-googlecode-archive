
/* Kruskal MST Algorithm
 *
 * Uses count-sort 
 *
 * $Id$
 */

#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "graph.h"

#ifndef NUMTESTS
#define NUMTESTS 10
#endif

extern void do_algorithm(graph_t *);

void perftest(int v, float ep) {
	int x = 0;
	double time = 0;
	struct timeval start, end;
	graph_t *g;

	g = gengraph(v, ep);
	for (x = 0; x < NUMTESTS; x++) {
		fprintf(stderr, "Run: %d\n", x);
		gettimeofday(&start, NULL);
		do_algorithm(g);
		gettimeofday(&end, NULL);

		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}
	freegraph(g);
	time /= 1000000.0;
	printf("Total time for %d runs: %f\n", x, time);
	printf("Average time for one run: %f\n", time / x);
}

int main(int argc, char **argv) {
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

	perftest(v, ep);

	return 0;
}
