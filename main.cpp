#include <mpi.h>
#include <queue>
#include <iostream>

#include "inc/Request.h"
#include "inc/ArgParsing.h"
#include "inc/Bum.h"

using namespace std;

int main(int argc, char** argv) {
	Parameters param;

	int size;
	int rank;

	if(parse(param, argc, argv) == -1)
		cout << "Za malo arg!" << endl;
	if(param.report)
		cout << "Gadam" << endl;

	MPI_Init(&argc,&argv);

	size=MPI::COMM_WORLD.Get_size();
	rank=MPI::COMM_WORLD.Get_rank();



	MPI_Finalize();

    return 0;
}
