#ifndef MPI_REQUEST_H
#define MPI_REQUEST_H

#include <mpi.h>

class MPIRequest {
    private:
        MPI_Datatype type;
        MPIRequest();

    public:
        MPI_Datatype& getType();
        static MPIRequest& getInstance();
};

#endif
