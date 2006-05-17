/* 
 * graph.c - jls
 *
 * $Id$
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

static int dfs_mark(graph_t *g, int *visit, int vertex);

/* Initialize the graph adjacency matrix */
void initgraph(graph_t *g) { 
	int x;
	g->matrix = malloc(g->numvert * sizeof(int));
	assert(g->matrix != NULL);
	for (x = 0; x < g->numvert; x++) {
		*(g->matrix + x) = malloc(g->numvert * sizeof(int));
		assert(*(g->matrix + x) != NULL);
		memset(*(g->matrix + x), 0, g->numvert * sizeof(int));
	}
}

static void randomize(graph_t *g) {
	int x, y;
	int weight;
	int prob = (int)(g->edgeprob * 100);

	for (x = 0; x < g->numvert; x++) {
		for (y = 0; y < g->numvert; y++) {
			if (y == x) continue;
			if (rand() % 100 < prob) {
				weight = rand() % g->numvert;
				g->matrix[x][y] = weight;
				//g->numedges++;
			}
		}
	}
}

static int connected(graph_t *g) {
	int *visit = malloc(g->numvert * sizeof(int));
	int count;
	int x; 

	memset(visit, 0, g->numvert * sizeof(int));
	count = dfs_mark(g, visit, 0); /* Start at vertex 0 */
	for (x = 0; x < g->numvert; x++) {
		if (*(visit + x) == 0)
			return 0;
	}

	free(visit);
	return 1;
}

static int dfs_mark(graph_t *g, int *visit, int vertex) {
	int x;
	int count = 0;
	if (*(visit + vertex) == 1) return 1;
	*(visit + vertex) = 1;

	for (x = 0; x < g->numvert; x++) {
		if (x == vertex) continue;
		if (g->matrix[vertex][x] > 0)
			count += dfs_mark(g, visit, x);
	}

	return count;
}

void freegraph(graph_t *g) {
	int x;
	for (x = 0; x < g->numvert; x++)
		free(g->matrix[x]);
	free(g->matrix);
	free(g);
}

/***
 * Public functions 
 ***/

graph_t* gengraph(int v, float ep, int seed) {
	graph_t *g;

	srand(seed);
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

	printgraph(g);

	return g;

}

void printgraph(graph_t* g) {
	int x, y;

	if (g->numvert > 10) return;

	printf("\n-------------\nThe graph's adjacency matrix is as follows:\n");

	for (x = 0; x < g->numvert; x++) {
		for (y = 0; y < g->numvert; y++)
			printf("%d   ", g->matrix[x][y]);
		printf("\n\n");
	}
}

#ifdef TEST
int main(int argc, char **argv) {
	int v;
	float ep;
	int seed;
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
		seed = atoi(*++argv);
	else
		seed = time(NULL);

	srand(seed);

	g = gengraph(v, ep);

	printf("V: %d\nEP: %f\n", v, ep);

	return 0;
}
#endif /* TEST */
