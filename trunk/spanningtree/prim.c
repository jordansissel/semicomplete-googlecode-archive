/* Prim MST Algorithm
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

typedef struct edge {
	int x, y;
	int weight;
} edge_t;

static void printedge(void *d) {
	edge_t *e = (edge_t *)d;
	printf("%d->%d", e->x, e->y);
}

#define ITEM(h,i) (*(*(h) + i))

typedef edge_t ** heap_t;

/* Heap-based priority queue 
 *
 * First element is actually an integer telling us where the last
 * element is. This is actually a clever hack, in my opinion.
 *
 * Heap needs to start at 1 anyway so that N*2 and N*2+1 are the right
 * indeces for the left and right children. To avoid unnecesasry
 * off-by-one math all over the place, we simply start index at 1.
 *
 * This lets us store some magic information in the 0th index.
 * We need to store the index of the last element anyway, right?
 * So, we might aswell store it in the 0th index.
 *
 * As such, heap[0].x is the index of the last element.
 */
void heapify(heap_t *heap, int index);
void buildheap(heap_t *heap);
void mkqueue(heap_t *heap, int size);
void enqueue(heap_t *heap, edge_t *e);
edge_t *dequeue(heap_t *heap);

void mkqueue(heap_t *heap, int size) {
	*heap = malloc(sizeof(edge_t*) * size);
	**heap = malloc(sizeof(edge_t));

	/* Hack, store item count in first "edge" x value */
	(**heap)->x = 0;
}

void enqueue(heap_t *heap, edge_t *e) {
	/* Add this new item to the end */
	*(*heap + (**heap)->x + 1) = e;
	((**heap)->x)++;
	buildheap(heap);
}

edge_t *dequeue(heap_t *heap) {
	edge_t *t = ITEM(heap, 1);
	if ((**heap)->x == 0)
		return NULL;
	ITEM(heap, 1) = ITEM(heap, (**heap)->x);
	(**heap)->x--;
	heapify(heap,1);
	return t;
}

void buildheap(heap_t *heap) {
	int x = (**heap)->x; /* last element index */

	while (x > 1) {
		/* Swap if less than */
		edge_t *t;

		if (ITEM(heap,x)->weight >= ITEM(heap,x / 2)->weight)
			break;

		t = ITEM(heap, x);
		ITEM(heap,x) = ITEM(heap, x / 2);
		ITEM(heap, x / 2) = t;
		x = x / 2;
	}
}

void heapify(heap_t *heap, int i) {
	int min;
	int size = (**heap)->x;
	int r = i * 2;
	int l = r + 1;

	/* Is left < self? */
	if (l <= size && ITEM(heap, l)->weight < ITEM(heap, i)->weight)
		min = l;
	else
		min = i;

	/* Is right < current minimum? */
	if (r <= size && ITEM(heap, r)->weight < ITEM(heap, min)->weight)
		min = r;

	/* If child < self, swap and try again with my new location */
	if (min != i) { /* Swap if we aren't the min */
		edge_t *t = ITEM(heap, i);
		ITEM(heap,i) = ITEM(heap, min);
		ITEM(heap,min) = t;
		heapify(heap, min);
	}
}

#define BITBLOCK (8)
#define MAPLEN(e) ((numedges / BITBLOCK) + 1)
#define SETBIT(f, e) (*((f) + (e / BITBLOCK)) |= 1 << (e % BITBLOCK))
#define GETBIT(f, e) ((*((f) + (e / BITBLOCK)) & (1 << (e % BITBLOCK))) > 0)

void prim_adjmatrix(graph_t *g) { /* {{{ */
	/* pick a random vertex to start */
	int v;
	int weight = 0;
	heap_t h;
	edge_t *e = NULL;

	/* quick-lookup bitmap of what verteces are in our tree */
	char *bitmap;
	bitmap = malloc(g->numvert / BITBLOCK);

	mkqueue(&h, g->numvert * g->numvert);

	v = rand() % g->numvert;

	do {
		int y;

		if (!GETBIT(bitmap, v)) {
			SETBIT(bitmap, v);

			if (e != NULL) {
				weight += e->weight;
				if (g->numvert <= 10)
					printf("%d -> %d (%d)\n", e->x, e->y, e->weight);
			}

			/* Add edges to heap */
			for (y = 0; y < g->numvert; y++) {
				if (WEIGHT(g, v, y) > 0) {
					edge_t *t;
					t = malloc(sizeof(edge_t));
					t->x = v;
					t->y = y;
					t->weight = WEIGHT(g, v, y);
					enqueue(&h, t);
				}
			}
		}

		/* Pop queue */

		if (e != NULL)
			free(e);
		e = dequeue(&h);
		if (e == NULL)
			break;
		if (!GETBIT(bitmap, e->x))
			v = e->x;
		else
			v = e->y; } while (1);

	printf("Total weight of MST for Prim: %d\n", weight);
} /* }}} */

void prim_adjlist(graph_t *g) { /* {{{ */
	/* pick a random vertex to start */
	int v;
	int x,y;
	int weight = 0;
	heap_t h;
	edge_t **list;
	edge_t *e = NULL;

	/* quick-lookup bitmap of what verteces are in our tree */
	char *bitmap;
	bitmap = malloc(g->numvert / BITBLOCK);

	mkqueue(&h, g->numvert * g->numvert);

	list = malloc(sizeof(edge_t *) * g->numvert);
	for (x = 0; x < g->numvert; x++) {
		*(list + x) = malloc(sizeof(edge_t) * g->numvert);
		for (y = 0; y < g->numvert; y++) {
			edge_t *t = *(list + x) + y;
			t->x = x;
			t->y = y;
			t->weight = WEIGHT(g, x, y);
		}
		(*(list + x) + y)->weight = -1;
	}

	v = rand() % g->numvert;

	do {
		int y;

		if (!GETBIT(bitmap, v)) {
			SETBIT(bitmap, v);

			if (e != NULL) {
				weight += e->weight;
				if (g->numvert <= 10)
					printf("%d -> %d (%d)\n", e->x, e->y, e->weight);
			}

			/* Add edges to heap */

			for (y = 0; (*(list + v) + y)->weight != -1; y++) {
				if (WEIGHT(g, v, y) > 0) {
					edge_t *t;
					t = malloc(sizeof(edge_t));
					t->x = v;
					t->y = y;
					t->weight = WEIGHT(g, v, y);
					enqueue(&h, t);
				}
			}
		}

		/* Pop queue */

		if (e != NULL)
			free(e);
		e = dequeue(&h);
		if (e == NULL)
			break;
		if (!GETBIT(bitmap, e->x))
			v = e->x;
		else
			v = e->y; 
	} while (1);

	for (x = 0; x < g->numvert; x++)
		free(*(list + x));
	free(list);

	printf("Total weight of MST for Prim: %d\n", weight);
} /* }}} */

#ifdef TEST
int main() {
	edge_t *e;
	heap_t h;
	int w = 0;
	graph_t *g;

	g = gengraph(5, .5, time(NULL));
	//prim_adjmatrix(g);
	prim_adjlist(g);
	freegraph(g);

	return 0;
}

int _main() {
	edge_t *e;
	heap_t h;
	int w = 0;

	srand(time(NULL));

	#define SIZE 10
	mkqueue(&h, SIZE);

	for (w = 0; w < SIZE; w++) {
		e = malloc(sizeof(edge_t));
		memset(e, 0, sizeof(edge_t));
		e->weight = rand() % 100;
		printf("Adding weight: %d\n", e->weight);
		enqueue(&h, e);
	}

	for (w = 1; w <= (*h)->x; w++) {
		edge_t *e;
		e = *(h + w);
		printf("Weight: %d\n", e->weight);
	}

	while ((*h)->x > 0) {
		printf("next: %d\n", dequeue(&h)->weight);
	}

	printf("\n");
}
#endif
