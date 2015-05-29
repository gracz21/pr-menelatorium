#include <mpi.h>
#include <set>
#include <iostream>
#include <unistd.h>

#include "../inc/HelpRequest.h"
#include "../inc/MPIRequest.h"
#include "../inc/Bum.h"
#include "../inc/Parameters.h"

using namespace std;

int main(int argc, char** argv) {
	int size,rank;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Parameters p;
    p.m = 1;
    p.s = 1;
    p.p = 1;
    int ids[1] = { 0 };

    Bum b(rank, 1, &p, ids, 0);
    b.run();
     

	MPI_Finalize();

    return 0;
}
