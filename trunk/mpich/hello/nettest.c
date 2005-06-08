#include "mpi.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void master();
void slave();

#include "log.h"

char myhost[150];
int myrank;

void debug(int level, const char *format, ...) {
	char str[1024];
	char host[150];
	va_list args;
	snprintf(str, 1024, "%s[%d]: %s", myhost, myrank, format);
	va_start(args,format);
	printf("\nTEST: ");
	printf(str, args);
	printf("\n\n");
	debuglog(level, str, args);
	va_end(args);
}


int main(int argc, char **argv) {
	int rank;
	char host[150];
	int namelen;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(host,&namelen);

	strcpy(myhost,host);
	myrank = rank;
	debug(0, "Hello!");
	printf("Hello world (Rank: %d / Host: %s)\n", rank, host);
	if (rank == 0) master();
	else slave();

	MPI_Finalize();
	return 0;
}

void master() {
	int nodes;
	int result[16];
	memset(result, 0, 16 * sizeof(int));

	MPI_Comm_size(MPI_COMM_WORLD, &nodes);
	debug(0, "EXAMPLE VALUE: %d", result[5]);
	debuglog(0, "EXAMPLE VALUE: %d", result[5]);

	while (1) {
		int x = 0;
		MPI_Status status;
		debug(0, "Waiting for data...");
		for (x = 1; x < nodes; x++) {
			MPI_Recv(result, 16, MPI_INT, x, 1, MPI_COMM_WORLD, &status);
			debug(0, "Got: %d (%d)", result[5], status.MPI_SOURCE);
		}
		sleep(1);
	}
}

void slave() {
	while (1) {
		int values[16];
		int x;
		for (x = 0; x < 16; x++) {
			values[x] = x;
		}

		debug(0, "Sending data...");
		MPI_Send(values, 16, MPI_INT, 0, 1, MPI_COMM_WORLD);
		sleep(1);
	}
}
	


