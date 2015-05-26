#include <stddef.h>

#include "../inc/MPIRequest.h"
#include "../inc/Request.h"

MPIRequest::MPIRequest() {
    const int numberOfProperties = 2;
    int blockLengths[2] = {1,1};
    MPI_Datatype typesOfProperties[2] = {MPI_INT, MPI_INT};

    MPI_Aint offsets[2];
    offsets[0] = offsetof(Request, processId);
    offsets[1] = offsetof(Request, timestamp);

    MPI_Type_create_struct(numberOfProperties, blockLengths, offsets, typesOfProperties, &type);
    MPI_Type_commit(&type);
}

MPIRequest::~MPIRequest() {
    MPI_Type_free(&type);
}

const MPI_Datatype& MPIRequest::getType() {
    return type;
}

const MPIRequest& MPIRequest::getInstance() {
    static MPIRequest instance;

    return instance;
}
