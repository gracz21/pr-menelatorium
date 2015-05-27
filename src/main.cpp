#include <mpi.h>
#include <queue>
#include <iostream>

#include "../inc/Request.h"
#include "../inc/MPIRequest.h"

using namespace std;

int main(int argc, char** argv) {
	int size,rank;
	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        cout << rank << endl;
        Request msg;
        msg.processId = 0;
        msg.timestamp = 10;
        msg.currentTime = 11;

        MPI_Send(&msg, 1, MPIRequest::getInstance().getType(), 1, 10, MPI_COMM_WORLD);
    } else {
        cout << rank << endl;
        MPI_Status status;
        Request msg;

        MPI_Recv(&msg, 1, MPIRequest::getInstance().getType(), 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        cout << msg.processId << endl << msg.timestamp << endl << msg.currentTime << endl;
    }

    MPI_Type_free(&(MPIRequest::getInstance().getType()));
	MPI_Finalize();

    return 0;
}
