#include <stddef.h>
#include <mpi.h>

#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

MPIRequest::MPIRequest() {
    const int numberOfProperties = 3;
    int blockLengths[3] = {1, 1, 1};
    MPI_Datatype typesOfProperties[3] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[3];
    offsets[0] = offsetof(Request, processId);
    offsets[1] = offsetof(Request, timestamp);
    offsets[2] = offsetof(Request, currentTime);

    MPI_Type_create_struct(numberOfProperties, blockLengths, offsets, typesOfProperties, &type);
    MPI_Type_commit(&type);
}

MPI_Datatype& MPIRequest::getType() {
    return type;
}

MPIRequest& MPIRequest::getInstance() {
    static MPIRequest instance;

    return instance;
}
