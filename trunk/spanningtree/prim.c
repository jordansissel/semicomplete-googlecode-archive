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

/* Prim's algorithm */
/* Pick a random vertex. 
 *
 * Add it's edges to the priority queue
 * Take the least-cost edge, add it's vertex.
 */

/* Heap-based priority queue */
/* First element is actually an integer telling us where the last
 * element is. It's a hack, but whatever :) */
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

	/* Swap tail into head */
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

#ifdef TEST
int main() {
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