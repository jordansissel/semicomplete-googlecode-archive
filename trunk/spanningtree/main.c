
/* Kruskal MST Algorithm
 *
 * Uses count-sort 
 *
 * $Id$
 */

#ifdef __FreeBSD__
#include <sys/time.h>
#include <sys/types.h>
#endif
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "graph.h"

#ifndef NUMTESTS
#define NUMTESTS 10
#endif

extern void kruskal_countsort(graph_t *g);
extern void kruskal_quicksort(graph_t *g);

void perftest(int v, float ep, void (*mst)(graph_t*), char *name) {
	int x = 0;
	double time = 0;
	struct timeval start, end;
	graph_t *g;

	for (x = 0; x < NUMTESTS; x++) {
#if DEBUG
		fprintf(stderr, "Starting run: %d\n", x);
#endif 
		g = gengraph(v, ep);
		gettimeofday(&start, NULL);
		mst(g);
		gettimeofday(&end, NULL);
		freegraph(g);

		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}

	time /= 1000000.0;
	printf("%s: Total time for %d runs: %f\n", name, x, time);
	printf("%s: Average time for one run: %f\n", name, time / x);
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

	perftest(v, ep, kruskal_countsort, "Kruskal w/ Count Sort");
	perftest(v, ep, kruskal_quicksort, "Kruskal w/ Quick Sort");

	return 0;
}
