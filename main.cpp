#include <mpi.h>
#include <queue>
#include <iostream>

#include "inc/Request.h"
#include "inc/Menel.h"

using namespace std;

int main(int argc, char** argv) {
    int size, rank, len;
	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Hello world: %d of %d na \n", rank, size);
	MPI_Finalize();

    return 0;
}
