#ifndef MPI_REQUEST_H
#define MPI_REQUEST_H

#include <mpi.h>

#include "MPICustomType.h"

class MPIRequest: public MPICustomType {
    private:
        MPIRequest();

    public:
        static MPIRequest& getInstance();
};

#endif
