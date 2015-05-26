#ifndef MPI_REQUEST_H
#define MPI_REQUEST_H

#include <mpi.h>

class MPIRequest {
    private:
        MPI_Datatype type;
        MPIRequest();
        ~MPIRequest();

    public:
        const MPI_Datatype& getType();
        static const MPIRequest& getInstance();
};

#endif
