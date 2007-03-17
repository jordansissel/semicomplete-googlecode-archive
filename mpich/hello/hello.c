#include "mpi.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int rank;
	char host[150];
	int namelen;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(host,&namelen);

	printf("Hello world (Rank: %d / Host: %s)\n", rank, host);

	MPI_Finalize();
	return 0;
}
