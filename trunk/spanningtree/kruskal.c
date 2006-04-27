
/* Kruskal MST Algorithm
 *
 * Uses count-sort 
 *
 * $Id$
 */

#include <stdio.h>
#include <assert.h>

#include "graph.h"
#include "linkedlist.h"

#define FORESTBLOCK (8)
#define MAPLEN(e) ((numedges / FORESTBLOCK) + 1)
#define SETFORESTBIT(f, e) (*((f) + (e / FORESTBLOCK)) |= 1 << (e % FORESTBLOCK))
#define GETFORESTBIT(f, e) ((*((f) + (e / FORESTBLOCK)) & (1 << (e % FORESTBLOCK))) > 0)

typedef struct edge {
	int x, y;
	int weight;
} edge_t;

typedef struct forest {
	char *bitmap;
	llnode_t *edges;
} forest_t;

void printedge(void *d) {
	edge_t *e = (edge_t *)d;
	printf("%d->%d", e->x, e->y);
}

/* Sort by weights, return total number of edges */
static int countsort(graph_t *g, edge_t **sortededges) {
	int max = 0;
	int x,y;
	int *countarr;

	edge_t *edges;

	int edgepiv = 0;
	int maxedges = g->numvert * g->numvert;

	/* Worst case, num edges is vert^2 */

	edges = malloc(maxedges * sizeof(edge_t));
	assert(edges != NULL);

	*sortededges = malloc(maxedges * sizeof(edge_t));
	assert(*sortededges != NULL);

	memset(edges, 0, maxedges * sizeof(edge_t));
	memset(*sortededges, 0, maxedges * sizeof(edge_t));

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
		memcpy(*sortededges + --countarr[edges[x].weight], edges + x, sizeof(edge_t));

	printf("EDGEPIV: %d\n", edgepiv);
	return edgepiv;
}

/* Kruskal Algorithm */
/* Algorithm:
 *
 * while edgelist is not empty
 *    remove minimum weight edge
 *
 */

void kruskal(graph_t *g, edge_t *edges, int numedges) {
	int i, j;
	forest_t *forests;
	llnode_t *mst = NULL;

	/* We'll use a bitmap for forests.
	 * An int can hold 32 (really, sizeof(int)*8) bits.
	 * Make an array of numvert/32 elements.
	 *
	 * Each bit correlates to a vertex.
	 *
	 * If a bit is high, then that vertex is in this forest.
	 *
	 * - Merging forests is simple, then: f1 | f2 == merged
	 * - Testing for cycles: if forest of v1 contains vertex v2, then adding edge
	 *   v1->v2 would produce a cycle
	 *
	 */

	for (i = 0; i < numedges; i++)
		printf("%d: %d -> %d (w: %d)\n", i, edges[i].x, edges[i].y, edges[i].weight);

	/* Create the forests. Initially, there are n forests (where n == numverteces) */
	forests = malloc(g->numvert * sizeof(forest_t));
	for (i = 0; i < g->numvert; i++) {
		/* Byte length of bitmap */
		int len = MAPLEN(numedges);

		forests[i].bitmap = malloc(len);
		forests[i].edges = NULL;

		memset(forests[i].bitmap, 0, len);
		SETFORESTBIT(forests[i].bitmap, i);
	}

	//printf("maplen: (edges %d) %d\n", numedges, MAPLEN(numedges));
	//printf("startmap: %02x\n", *(forests[0].bitmap));

	for (i = 0; i < numedges; i++) {
		/* edges[i] == lowest weight edge. */
		forest_t *xforest = (forests + edges[i].x);
		forest_t *yforest = (forests + edges[i].y);

		/* Does forest x include vertex y? Merge if not. */
		if (!GETFORESTBIT(xforest->bitmap, edges[i].y)
			 && !GETFORESTBIT(yforest->bitmap, edges[i].x)) {

			/* Indicate that vertex y is now in forest x */
			printf("\n");
			printf("(%d->%d) Exists in x,y map: %d,%d (%02x/%02x)\n", edges[i].x, edges[i].y,
					 GETFORESTBIT(xforest->bitmap, edges[i].y),
					 GETFORESTBIT(yforest->bitmap, edges[i].x),
					 *(xforest->bitmap),
					 *(yforest->bitmap));
			SETFORESTBIT(xforest->bitmap, edges[i].y);
			printf("Adding edge: %d->%d (%d)\n", edges[i].x, edges[i].y, edges[i].weight);
			fflush(stdout);

			printf("before xforest: "); printlist(xforest->edges, &printedge);
			printf("before yforest: "); printlist(yforest->edges, &printedge);
			//printf("before mst: "); printlist(mst, &printedge);

			/* Add the edge to the end */
			llmerge(&(xforest->edges), &(yforest->edges));
			lladd(&(xforest->edges), edges + i);

			/* Merge the maps */
			for (j = 0; j <= MAPLEN(numedges); j++) 
			  *(yforest->bitmap + j) = *(xforest->bitmap + j) |= *(yforest->bitmap + j);
			memcpy(yforest->bitmap, xforest->bitmap, MAPLEN(numedges));

			yforest->edges = xforest->edges;
			for (j = 0; j < g->numvert; j++) {
				forest_t *f = (forests + j);

				/* Merge forest pointers */
				if (GETFORESTBIT(f->bitmap, edges[i].x) ||
					 GETFORESTBIT(f->bitmap, edges[i].y)) {
					if (f->edges != xforest->edges)
						free(f->edges);
					f->edges = xforest->edges;
				}
			}

			printf("after xforest: "); printlist(xforest->edges, &printedge);
			printf("after yforest: "); printlist(yforest->edges, &printedge);
		} else {
			//printf("SKIPPING: %d->%d (%d)\n", edges[i].x, edges[i].y, edges[i].weight);
		}
	}

	fflush(stdout);
	//printf("endmap: %08x\n", *(forests[0].bitmap));
	/* At this point, all edges will have the same forest becuase we have a connected graph */
	if (1) {
		int x = 0;
		//graph_t mst;
		llnode_t *e;
		//mst.numvert = g->numvert;
		//initgraph(&mst);

		e = forests[0].edges;
		while (e != NULL) {
			edge_t *data = (edge_t *) e->data;
			printf("%d => %d (%d) [%08x <=> %08x]\n", data->x, data->y, data->weight, e, e->next);
			//printf("foo: %d\n", ((edge_t *)e->data) - edges);
			//mst.matrix[data->x][data->y] = data->weight;
			e = e->next;
			fflush(stdout);
			x++;
			if (x > 5) break;
		}

		//printgraph(&mst);
	}
}

void do_algorithm(graph_t *g) {
	edge_t *sortededges;
	int numedges;

	numedges = countsort(g, &sortededges);
	kruskal(g, sortededges, numedges);
}


