#include <mpi.h>
#include <queue>
#include <iostream>

#include "../inc/Request.h"
#include "../inc/Init.h"
#include "../inc/Bum.h"
#include "../inc/MuseumService.h"

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
            Bum bum(rank, 1, &params, bumsIds);
            bum.run();
        }
    } else if(rank == 0) { 
        if(st == -1) {
            cout << "Podano za malo parametrow!";
        } else {
            cout << "Parametry nie spelniaja zalozen zadania!";
        }
    }

    MPI_Finalize();


    return 0;
}
