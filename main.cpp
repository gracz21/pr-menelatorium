#include <mpi.h>
#include <queue>
#include <iostream>

#include "inc/Request.h"
#include "inc/ArgParsing.h"
#include "inc/Bum.h"

using namespace std;

int main(int argc, char** argv) {
	MPI_Init(&argc,&argv);

	MPI_Finalize();

    return 0;
}
