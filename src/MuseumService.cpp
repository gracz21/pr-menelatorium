#include "../inc/MuseumService.h"
#include "../inc/messages.h"
#include "../inc/Request.h"
#include "../inc/MPIRequest.h"

MuseumService::MuseumService() {
    time = 0;
}

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
    time = ((time > inRequest.currentTime) ? time : inRequest.currentTime) + 1;

    outRequest.currentTime = ++time;
	MPI_Send(&outRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, EXPO_START_CONFIRMATION, MPI_COMM_WORLD);
}

void MuseumService::endExposition() {
	Request request;
	MPI_Status status;

	MPI_Recv(&request, 1, MPIRequest::getInstance().getType(), MPI_ANY_SOURCE, SINGLE_EXIT_NOTIFICATION, MPI_COMM_WORLD, &status);
    time = ((time > request.currentTime) ? time : request.currentTime) + 1;
}
