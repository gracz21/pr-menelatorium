#include "../inc/MPICustomType.h"

MPICustomType::MPICustomType() {

}

MPI_Datatype& MPICustomType::getType() {
    return type;
}
