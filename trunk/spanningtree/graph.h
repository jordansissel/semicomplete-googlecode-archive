/* $Id$ */

typedef struct graph {
	int numvert;
	float edgeprob;
	int **matrix;
} graph_t;

/* Return a generated graph of size rows x cols */
graph_t * gengraph(int v, float ep);
