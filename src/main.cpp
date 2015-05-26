#include <mpi.h>
#include <queue>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
	MPI_Init(&argc,&argv);

	MPI_Finalize();

    return 0;
}
