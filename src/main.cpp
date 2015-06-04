#include <mpi.h>
#include <queue>
#include <iostream>

#include "../inc/Request.h"
#include "../inc/Init.h"
#include "../inc/Bum.h"
#include "../inc/MuseumService.h"
#include "../inc/MPIRequest.h"
#include "../inc/MPIHelpRequest.h"

using namespace std;

int main(int argc, char** argv) {
    Parameters params;
    int size, rank;

    MPI_Init(&argc,&argv);

    size = MPI::COMM_WORLD.Get_size();
    rank = MPI::COMM_WORLD.Get_rank();

    int st = parseParameters(params, argc, argv, size);
    if(st == 0) {
        int *bumsIds = createBumsIds(params.m);
        if(rank == 0) {
            MuseumService museumService;
            museumService.run();
        } else {
            Bum bum(rank, randWeight(rank), &params, bumsIds);
            bum.run();
        }
    } else if(rank == 0) { 
        if(st == -1) {
            cout << "Podano za mało parametrów!";
        } else {
            cout << "Parametry nie spełniają założen zadania!";
        }
    }

    MPI_Type_free(&(MPIRequest::getInstance().getType()));
    MPI_Type_free(&(MPIHelpRequest::getInstance().getType()));

    MPI_Finalize();


    return 0;
}
