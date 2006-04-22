/* $Id$ */

typedef struct graph {
	int rows;
	int cols;
	int **adjacency;
} graph_t;

/* Return a generated graph of size rows x cols */
graph_t * gengraph(int rows, int cols);
