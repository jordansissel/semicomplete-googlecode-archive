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


