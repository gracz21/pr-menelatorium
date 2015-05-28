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

    museumAttendanceList = new int[worldParameters->s];
}

Bum::~Bum() {
    delete [] museumAttendanceList;
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
    // TODO: first critical section
}

void Bum::participateInExposition() {
    cout << "Proces: " << id << ", czas: " << time << " - na wystawie" << endl;
    time++;

    bool gotDrunk = ((rand() % 10) <= 4);

    if (gotDrunk) {
        helpRequests.clear();
        callForHelp(); 
        waitForHelp();
    }

    leaveMuseum();
}

void Bum::callForHelp() {
    int timeWhenGotDrunk = time;
    HelpRequest helpRequestBuffers[worldParameters->s - 1];
    int helpRequestsIterator = 0;
    
    for (int i = 0; i < worldParameters->s; i++) {
        if (museumAttendanceList[i] != id) {
            time++;

            helpRequestBuffers[helpRequestsIterator].processId = id;
            helpRequestBuffers[helpRequestsIterator].timestamp = timeWhenGotDrunk;;
            helpRequestBuffers[helpRequestsIterator].currentTime = time;
            helpRequestBuffers[helpRequestsIterator].weight = weight;

            MPI_Request status;
            MPI_Isend(&helpRequestBuffers[helpRequestsIterator], 1, MPIRequest::getInstance().getType(), museumAttendanceList[i], HELP_REQ, 
                      MPI_COMM_WORLD, &status);
            MPI_Request_free(&status);

            helpRequestsIterator++;
        }
    }
}

void Bum::waitForHelp() {
    int remainingResponsesNumber = worldParameters->s - 1;
    
    while (remainingResponsesNumber > 0) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == ENTER_REQ) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            delayedEnterRequests.push_back(enterRequest);

        } else if (status.MPI_TAG == EXIT_NOTIFICATION) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        } else if (status.MPI_TAG == HELP_REQ) {

        } else if (status.MPI_TAG == HELP_RESP) {
            remainingResponsesNumber--;
        } else {
            throw "Unexpected message";
        }
    }
}

void Bum::leaveMuseum() {
     
}
