/* $Id$ */

typedef struct graph {
	int numvert;
	float edgeprob;
	int **matrix;
} graph_t;

/* Get the weight at a given index */
#define WEIGHT(g, x, y) (g->matrix[x][y])

/* Return a generated graph of size rows x cols */
graph_t * gengraph(int v, float ep);
