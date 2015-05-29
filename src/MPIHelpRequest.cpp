#include <stddef.h>
#include <mpi.h>

#include "../inc/MPIHelpRequest.h"
#include "../inc/HelpRequest.h"

MPIHelpRequest::MPIHelpRequest() {
    const int numberOfProperties = 4;
    int blockLengths[4] = {1, 1, 1, 1};
    MPI_Datatype typesOfProperties[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[4];
    offsets[0] = offsetof(HelpRequest, processId);
    offsets[1] = offsetof(HelpRequest, timestamp);
    offsets[2] = offsetof(HelpRequest, currentTime);
    offsets[3] = offsetof(HelpRequest, weight);

    MPI_Type_create_struct(numberOfProperties, blockLengths, offsets, typesOfProperties, &type);
    MPI_Type_commit(&type);
}

MPIHelpRequest& MPIHelpRequest::getInstance() {
    static MPIHelpRequest instance;

    return instance;
}
