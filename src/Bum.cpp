#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "../inc/Bum.h"
#include "../inc/messages.h"
#include "../inc/Request.h"
#include "../inc/MPIRequest.h"

using namespace std;

Bum::Bum(int id, unsigned short weight, const Parameters *worldParameters, int time) {
    this->id = id;
    this->weight = weight;
    this->worldParameters = worldParameters;
    this->time = time;
}

int Bum::getId() {
    return id;
}

void Bum::run() {
    hangAround();
    goToMuseum();
    participateInExposition();
}

void Bum::hangAround() {
    bool wantsToGoToMuseum = false;

    while (!wantsToGoToMuseum) {
        cout << "Proces: " << id << ", czas: " << time << " - włóczy się po mieście" << endl;
        time++;

        checkForIncommingMessages();
        wantsToGoToMuseum = ((rand() % 10) <= 4);
    }
}

void Bum::checkForIncommingMessages() {
    int messageAvailable = 0;
    MPI_Status status;

    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageAvailable, &status);

    if (messageAvailable) {
        handleMessageWhenIdle();
    }
}

void Bum::handleMessageWhenIdle() {
    Request request;

    MPI_Status status;
    MPI_Recv(&request, 1, MPIRequest::getInstance().getType(), MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    time = (time > request.currentTime) ? time : request.currentTime;
    time++;

    if (status.MPI_TAG == ENTER_REQ) {
        time++;
        Request response(-1, -1, time);
        MPI_Send(&response, 1, MPIRequest::getInstance().getType(), request.processId, ENTER_RESP, MPI_COMM_WORLD);
    }
}

void Bum::goToMuseum() {

}

void Bum::participateInExposition() {
    cout << "Proces: " << id << ", czas: " << time << " - na wystawie" << endl;
    time++;

    bool gotDrunk = ((rand() % 10) <= 4);

    if (gotDrunk) {
        callForHelp(); 
    }

    leaveMuseum();
}

void Bum::callForHelp() {

}

void Bum::leaveMuseum() {

}
