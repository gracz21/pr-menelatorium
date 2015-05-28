#include <mpi.h>
#include <set>
#include <iostream>
#include <unistd.h>

#include "../inc/HelpRequest.h"
#include "../inc/MPIRequest.h"

using namespace std;

void snt() {
    int numbers[10];

    for (int i = 0; i < 10; i++) {
        numbers[i] = i;
        MPI_Request status;
        MPI_Isend(&numbers[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        MPI_Request_free(&status);
    }
}

int main(int argc, char** argv) {
	int size,rank;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        snt();        
    } else {
        sleep(5);

        for (int i = 0; i < 10; i++) {
            int nr;
            MPI_Recv(&nr, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            cout << nr << endl;
        }
    }

	MPI_Finalize();

    return 0;
}
