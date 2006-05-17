/* $Id$ */

typedef struct graph {
	int numvert;
	float edgeprob;
	int **matrix;
	//int numedges;
} graph_t;

/* Get the weight at a given index */
#define WEIGHT(g, x, y) (g->matrix[x][y])

/* Return a generated graph of size rows x cols */
graph_t * gengraph(int v, float ep, int seed);

/* Initialize a graph matrix */
void initgraph(graph_t *g);

/* Clean up allocations by initgraph */
void freegraph(graph_t *g);

/* Printy! */
void printgraph(graph_t* g);
