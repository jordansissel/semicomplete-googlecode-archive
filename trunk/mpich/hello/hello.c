#include "mpi.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Rank: %d\n", rank);
	system("uname -a");

	MPI_Finalize();
	return 0;
}
