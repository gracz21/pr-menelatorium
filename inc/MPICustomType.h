#ifndef MPI_CUSTOM_TYPE_H
#define MPI_CUSTOM_TYPE_H

#include <mpi.h>

class MPICustomType {
    protected:
        MPICustomType();
        MPI_Datatype type;

    public:
        MPI_Datatype& getType();
};

#endif
