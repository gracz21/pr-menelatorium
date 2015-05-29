#include "../inc/MuseumService.h"
#include "../inc/messages.h"
#include "../inc/Request.h"
#include "../inc/MPIRequest.h"

void MuseumService::run() {
	while(true) {
		startExposition();
		endExposition();
	}
}

void MuseumService::startExposition() {
	Request inRequest;
	Request outRequest;
	MPI_Status status;
	MPI_Recv(&inRequest, 1, MPIRequest::getInstance().getType(), MPI_ANY_SOURCE, EXPO_START, MPI_COMM_WORLD, &status);
	MPI_Send(&outRequest, 1, MPIRequest::getInstance().getType(), inRequest.processId, EXPO_START_CONFIRMATION, MPI_COMM_WORLD);
}

void MuseumService::endExposition() {
	Request Request;
	MPI_Status status;
	MPI_Recv(&inRequest, 1, MPIRequest::getInstance().getType(), MPI_ANY_SOURCE, EXIT_NOTIFICATION, MPI_COMM_WORLD, &status);
}