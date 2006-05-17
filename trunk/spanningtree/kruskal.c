/* Kruskal MST Algorithm
 *
 * Uses count-sort 
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graph.h"
#include "carp.h"

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
	edge_t *edges;
	int nextedge;
} forest_t;

static void printedge(void *d) {
	edge_t *e = (edge_t *)d;
	printf("%d->%d", e->x, e->y);
}

static void do_qsort(edge_t *edges, int start, int end);
static int qsort_partition(edge_t *edges, int start, int end);

/* Count sort - Sort by weights, return total number of edges */
static int countsort(graph_t *g, edge_t **sortededges) { /* {{{ */
	int max = 0;
	int x,y;
	int *countarr;

	edge_t *edges;

	int edgepiv = 0;
	int maxedges = g->numvert * g->numvert;

	/* Worst case, num edges is vert^2 */
	edges = malloc(maxedges * sizeof(edge_t));
	assertmsg(edges != NULL, "malloc failed");
	memset(edges, 0, maxedges * sizeof(edge_t));

	//*sortededges = malloc(maxedges * sizeof(edge_t));
	//assert(*sortededges != NULL);
	//memset(*sortededges, 0, maxedges * sizeof(edge_t));

	/* We know that the max weight is the number of verteces */
	max = g->numvert + 1;

	countarr = malloc(sizeof(int) * max);
	assertmsg(countarr != NULL, "malloc failed");
	memset(countarr, 0, sizeof(int) * max);

	/* Make the counts */
	for (x = 0; x < g->numvert; x++)
		for (y = 0; y < g->numvert; y++) {
			int w = WEIGHT(g,x,y);
			if (w > 0) {
				assert(w < max);

				countarr[w]++;

				/* While we're here, add this edge to the list of edges */
				edges[edgepiv].weight = w;
				edges[edgepiv].x = x;
				edges[edgepiv].y = y;
				edgepiv++;
			}
		}

	/* Make countarr cumulative */
	for (x = 1; x < max; x++)
		countarr[x] += countarr[x - 1];

	/* Now that we know how many edges we have... */
	*sortededges = malloc((edgepiv + 1) * sizeof(edge_t));
	assertmsg(*sortededges != NULL, "malloc failed");
	//memset(*sortededges, 0, countarr[* sizeof(edge_t));

	/* Sort the edges by weight */
	/* XXX: Solaris crashes on free(edges) if we go to x <= edgepiv. */
	for (x = 0; x < edgepiv; x++, countarr[edges[x].weight]--) {
		memcpy(*sortededges + countarr[edges[x].weight], edges + x, sizeof(edge_t));
	}

	free(countarr);
	free(edges);
	return edgepiv;
} /* }}} */

/* Quick sort - Sort by weights, return total number of edges */
static int quicksort(graph_t *g, edge_t **sortededges) { /* {{{ */
	int x,y;
	int edgecnt = 0;
	int maxedges = g->numvert * g->numvert;

	/* Worst case, num edges is vert^2 */
	*sortededges = malloc(maxedges * sizeof(edge_t));
	assertmsg(*sortededges != NULL, "malloc failed");
	memset(*sortededges, 0, maxedges * sizeof(edge_t));

	/* Create the list of edges */
	for (x = 0; x < g->numvert; x++)
		for (y = 0; y < g->numvert; y++)
			if (WEIGHT(g, x, y) > 0) {
				edge_t *e = *sortededges + edgecnt;
				e->weight = WEIGHT(g, x, y);
				e->x = x;
				e->y = y;
				edgecnt++;
			}

	do_qsort(*sortededges, 0, edgecnt);

	return edgecnt;
}
/* }}} */

/* Quick sort implementation {{{ */
static void do_qsort(edge_t *edges, int start, int end) {
	int piv;
	if (start < end) {
		piv = qsort_partition(edges, start, end);
		do_qsort(edges, start, piv - 1);
		do_qsort(edges, piv + 1, end);
	}
}

static int qsort_partition(edge_t *edges, int start, int end) {
	int gt, lt;
	int piv = start;
	edge_t t;

	gt = end;
	lt = start;

	do {
		/* Find elements worth swapping */
		while (edges[lt].weight <= edges[piv].weight && lt < end)
			lt++;
		while (edges[gt].weight > edges[piv].weight && gt > start)
			gt--;

		/* Swap two elements, if it makes sense */
		if (gt > lt) {
			memcpy(&t, edges + lt, sizeof(edge_t));
			memcpy(edges + lt, edges + gt, sizeof(edge_t));
			memcpy(edges + gt, &t, sizeof(edge_t));
		}

	} while (gt > lt);

	/* Move pivot to middle */
	memcpy(&t, edges + piv, sizeof(edge_t));
	memcpy(edges + piv, edges + gt, sizeof(edge_t));
	memcpy(edges + gt, &t, sizeof(edge_t));

	/* Return final position of pivot */
	return gt;
}
/* end quicksort }}} */

/* Kruskal Algorithm */
void kruskal(graph_t *g, edge_t *edges, int numedges) { /* {{{ */
	int i, j;
	int mstweight = 0;
	forest_t *forests;

	/* We'll use a bitmap for forests.
	 * An int can hold 32 (really, sizeof(int)*8) bits.
	 * Make an array of numvert/32 elements.
	 *
	 * Each bit correlates to a vertex.
	 *
	 * If a bit is high, then that vertex is in this forest.
	 */

	/* Create the forests. Initially, there are n forests (where n == numverteces) */
	forests = malloc(g->numvert * sizeof(forest_t));
	assertmsg(forests != NULL, "malloc failed");
	for (i = 0; i < g->numvert; i++) {
		/* Byte length of bitmap */
		int len = MAPLEN(numedges);

		forests[i].bitmap = malloc(len);
		assertmsg(forests[i].bitmap != NULL, "malloc failed");
		forests[i].edges = malloc(numedges * sizeof(edge_t));
		assertmsg(forests[i].edges != NULL, "malloc failed");
		forests[i].nextedge = 0;

		//fprintf(stderr, "forest %d: %d, %d\n", i, forests[i].bitmap, forests[i].edges);

		memset(forests[i].bitmap, 0, len);
		memset(forests[i].edges, 0, numedges * sizeof(edge_t));
		SETFORESTBIT(forests[i].bitmap, i);
	}

	/* Loop over all sorted edges. Add each edge unless it creates a cycle */
	for (i = 0; i < numedges; i++) {
		/* edges[i] == lowest weight edge. */
		forest_t *xforest = (forests + edges[i].x);
		forest_t *yforest = (forests + edges[i].y);

		if (xforest->bitmap == NULL)
			fprintf(stderr, "chkforest %d: %d\n", edges[i].x, xforest->bitmap);
		if (xforest->edges == NULL)
			fprintf(stderr, "chkforest edg %d: %d\n", edges[i].x, xforest->edges);
		/* Does forest x include vertex y? Merge if not. */
		if (!GETFORESTBIT(xforest->bitmap, edges[i].y)) {
			/* Indicate that vertex y is now in forest x */
			SETFORESTBIT(xforest->bitmap, edges[i].y);

			/* Copy edge to this tree */
			memcpy(xforest->edges + xforest->nextedge,edges + i,sizeof(edge_t));
			xforest->nextedge++;

			/* Merge edges from forest y into forest x lists */
			for (j = 0; j < yforest->nextedge; j++)
				memcpy(xforest->edges + xforest->nextedge++,yforest->edges + j,sizeof(edge_t));

			for (j = 0; j <= MAPLEN(numedges); j++) 
			  *(yforest->bitmap + j) = *(xforest->bitmap + j) |= *(yforest->bitmap + j);

			/* Copy forest x to all forests with vertex X or Y */
			for (j = 0; j < g->numvert; j++) {
				forest_t *f = (forests + j);

				/* Merge forest pointers */
				if (j == edges[i].x) continue;

				if (GETFORESTBIT(f->bitmap, edges[i].x) ||
					 GETFORESTBIT(f->bitmap, edges[i].y)) {
					/* XXX:
					 * Failure to free edges and bitmap here will cause some
					 * serious memory leaking and crashes. Hurray for finding this bug.
					 */
					if (f->edges != xforest->edges) {
						free(f->edges);
						free(f->bitmap);
					}
					f->edges = xforest->edges;
					f->bitmap = xforest->bitmap;
					f->nextedge = xforest->nextedge;
				}
			}
		}
	}

	/* At this point, we are done with the algorithm */

	#define PRINTEDGES(f) do { \
		int z; \
		for (z = 0; z < (f)->nextedge; z++) \
		printf("%d->%d (weight: %d)\n", (f)->edges[z].x, (f)->edges[z].y, (f)->edges[z].weight); \
	} while (0)


	if (g->numvert <= 10) {
		PRINTEDGES(forests);
		mstweight = 0;

		for (i = 0; i < forests[0].nextedge; i++) {
			edge_t *e = forests[0].edges + i;
			mstweight += e->weight;
		}

		printf("Total weight of MST using Kruskal: %d\n", mstweight);
		printf("\n");
	}

	/* Every forest should be the same, so we only need to free one bitmap and edge */
	free(forests[0].bitmap);
	free(forests[0].edges);

	/* Blow away the forest set */
	free(forests);
} /* }}} */

void kruskal_countsort(graph_t *g) {
	edge_t *sortededges;
	int numedges;

	numedges = countsort(g, &sortededges);
	kruskal(g, sortededges, numedges);

	/* cleanup  */
	free(sortededges);
}

void kruskal_quicksort(graph_t *g) {
	edge_t *sortededges;
	int numedges;

	numedges = quicksort(g, &sortededges);
	kruskal(g, sortededges, numedges);

	/* cleanup  */
	free(sortededges);
}
