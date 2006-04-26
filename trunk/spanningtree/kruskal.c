
/* Kruskal MST Algorithm
 *
 * Uses count-sort 
 *
 * $Id$
 */

#include <stdio.h>
#include <assert.h>

#include "graph.h"

typedef struct edge {
	int x, y;
	int weight;
} edge_t;

/* Sort by weights */
static void kruskal_countsort(graph_t *g) {
	int max = 0;
	int x,y;
	int *countarr;

	edge_t *edges;
	edge_t *sortededges;

	int edgepiv = 0;

	int maxedges = g->numvert * g->numvert;

	/* Worst case, num edges is vert^2 */

	edges = malloc(maxedges * sizeof(edge_t));
	assert(edges != NULL);

	sortededges = malloc(maxedges * sizeof(edge_t));
	assert(sortededges != NULL);

	memset(edges, 0, maxedges * sizeof(edge_t));
	memset(sortededges, 0, maxedges * sizeof(edge_t));

	/* Find max weight */
	for (x = 0; x < g->numvert; x++)
		for (y = 0; y < g->numvert; y++)
			if (max < WEIGHT(g,x,y))
				max = WEIGHT(g,x,y);

	/* Bump for 0 weights aswell */
	max++;

	countarr = malloc(sizeof(int) * max);
	assert(countarr != NULL);

	memset(countarr, 0, sizeof(int) * max);

	/* Make the counts */
	for (x = 0; x < g->numvert; x++)
		for (y = 0; y < g->numvert; y++)
			if (WEIGHT(g,x,y) > 0) {
				countarr[WEIGHT(g,x,y)]++;

				/* While we're here, add this edge to the list of edges */
				edges[edgepiv].weight = WEIGHT(g,x,y);
				edges[edgepiv].x = x;
				edges[edgepiv].y = y;
				edgepiv++;
			}

	/* Make countarr cumulative */
	for (x = 1; x < max; x++)
		countarr[x] += countarr[x - 1];

	/* Sort the edges by weight */
	/* We use --countarr[...] becuase it's off by one, so subtract early. */
	for (x = 0; x <= edgepiv; x++)
		memcpy(sortededges + --countarr[edges[x].weight], edges + x, sizeof(edge_t));

	for (x = 0; x < edgepiv; x++)
		printf("%d -> %d (w: %d)\n", sortededges[x].x, sortededges[x].y, sortededges[x].weight);

	/* Begin Kruskal Algorithm */
	// and... naptime
}

void do_algorithm(graph_t *g) {
	kruskal_countsort(g);
}
