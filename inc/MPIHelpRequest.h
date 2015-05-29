#ifndef MPI_HELP_REQUEST_H
#define MPI_HELP_REQUEST_H

#include <mpi.h>

#include "MPICustomType.h"

class MPIHelpRequest: public MPICustomType {
    private:
        MPIHelpRequest();

    public:
        static MPIHelpRequest& getInstance();
};

#endif
