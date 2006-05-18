
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

static int numtests;

extern void kruskal_countsort(graph_t *g);
extern void kruskal_quicksort(graph_t *g);
extern void prim_adjmatrix(graph_t *g);

void perftest(int v, float ep, void (*mst)(graph_t*), char *name, int seed) {
	int x = 0;
	double time = 0;
	struct timeval start, end;
	graph_t *g;

	for (x = 0; x < numtests; x++) {
#if DEBUG
		fprintf(stderr, "Starting run: %d\n", x);
#endif 
		g = gengraph(v, ep, seed + x);
		gettimeofday(&start, NULL);
		mst(g);
		gettimeofday(&end, NULL);
		freegraph(g);

		time += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	}

	time /= 1000000.0;
	if (x > 1)
		printf("%s: Average time for one run: %f\n", name, time / x);
	else 
		printf("%s: Total time for %d run%s: %f\n", name, x, (x > 1) ? "s" : "", time);
}

int main(int argc, char **argv) {
	int v;
	int seed;
	float ep;

	//if (sscanf(*++argv, "%d", &v) == 0) {
	if (fscanf(stdin, "%d\n", &v) == 0) {
		fprintf(stderr,"First argument is bad.\n");
		return 1;
	}

	//if (sscanf(*++argv, "%f", &ep) == 0) {
	if (fscanf(stdin, "%f", &ep) == 0) {
		fprintf(stderr,"Second argument is bad.\n");
		return 1;
	}

	if (argc >= 2)
		numtests = atoi(*++argv);
	else
		numtests = 10;

	assert(numtests > 0);

	if (argc >= 3)
		seed = atoi(*++argv);
	else
		seed = time(NULL);

	perftest(v, ep, kruskal_countsort, "Kruskal w/ Count Sort", seed);
	perftest(v, ep, kruskal_quicksort, "Kruskal w/ Quick Sort", seed);
	perftest(v, ep, prim_adjmatrix, "Prim w/ Adjacency Matrix", seed);

	return 0;
}
