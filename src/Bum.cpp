#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "../inc/Bum.h"
#include "../inc/messages.h"
#include "../inc/Request.h"
#include "../inc/MPIRequest.h"
#include "../inc/MPIHelpRequest.h"

using namespace std;

Bum::Bum(int id, unsigned short weight, const Parameters *worldParameters, int *bumsIds, int time) {
    this->id = id;
    this->weight = weight;
    this->worldParameters = worldParameters;
    this->time = time;
    this->bumsIds = bumsIds;

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
        handleMessageWhenIdle(status);
    }
}

void Bum::handleMessageWhenIdle(MPI_Status &status) {
    if (status.MPI_TAG == ENTER_REQ) {
        Request enterRequest;
        MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

        time++;
        Request response(-1, -1, time);
        MPI_Send(&response, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, ENTER_RESP, MPI_COMM_WORLD);

    } else if (status.MPI_TAG == EXIT_NOTIFICATION) {
        Request exitNotification;
        MPI_Recv(&exitNotification, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time = ((time > exitNotification.currentTime) ? time : exitNotification.currentTime) + 1;

    } else if (status.MPI_TAG == HELP_REQ) {
        HelpRequest helpRequest;
        MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

        HelpRequest response(-1, -1, ++time, -1);
        MPI_Send(&response, 1, MPIHelpRequest::getInstance().getType(), helpRequest.processId, HELP_RESP, MPI_COMM_WORLD);

    } else if (status.MPI_TAG == NURSE_RELEASE_NOTIFICATION) {
        HelpRequest helpRequest;
        MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

    } else {
        throw "Unexpected message";
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
        callForHelp(); 
        waitForHelp();
    }

    exitNotifications.clear();
    leaveMuseum();
}

void Bum::callForHelp() {
    helpRequests.clear();
    helpRequestsFilter.clear();
    helpRequests.insert(HelpRequest(id, time, time, weight));
    myHelpRequest = &(*helpRequests.begin());

    int timeWhenGotDrunk = time;
    HelpRequest helpRequestBuffers[worldParameters->s - 1];
    int helpRequestsIterator = 0;

    for (unsigned int i = 0; i < worldParameters->s; i++) {
        if (museumAttendanceList[i] != id) {
            time++;

            helpRequestBuffers[helpRequestsIterator].processId = id;
            helpRequestBuffers[helpRequestsIterator].timestamp = timeWhenGotDrunk;;
            helpRequestBuffers[helpRequestsIterator].currentTime = time;
            helpRequestBuffers[helpRequestsIterator].weight = weight;

            MPI_Request status;
            MPI_Isend(&helpRequestBuffers[helpRequestsIterator], 1, MPIHelpRequest::getInstance().getType(), museumAttendanceList[i], HELP_REQ, 
                      MPI_COMM_WORLD, &status);
            MPI_Request_free(&status);

            helpRequestsIterator++;
        }
    }
}

void Bum::waitForHelp() {
    unsigned int remainingResponsesNumber = worldParameters->s - 1;
    bool served = false;
    
    while (!served) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == ENTER_REQ) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

            delayedEnterRequests.push_back(enterRequest);

        } else if (status.MPI_TAG == EXIT_NOTIFICATION) {
            Request exitNotification;
            MPI_Recv(&exitNotification, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > exitNotification.currentTime) ? time : exitNotification.currentTime) + 1;

            if (id == museumAttendanceList[worldParameters->s - 1]) {
                exitNotifications.push_back(exitNotification);
            }

        } else if (status.MPI_TAG == HELP_REQ) {
            HelpRequest helpRequest;
            MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

            insertHelpRequest(helpRequest);
            HelpRequest response = *myHelpRequest;
            response.currentTime = ++time;
            MPI_Send(&response, 1, MPIHelpRequest::getInstance().getType(), helpRequest.processId, HELP_RESP, MPI_COMM_WORLD);

        } else if (status.MPI_TAG == HELP_RESP) {
            HelpRequest helpRequest;
            MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

            if (helpRequest.processId != -1) {
                insertHelpRequest(helpRequest);
            }

            remainingResponsesNumber--;

        } else if (status.MPI_TAG == NURSE_RELEASE_NOTIFICATION) {
            HelpRequest helpRequest;
            MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

            helpRequestsFilter.insert(helpRequest);
            helpRequests.erase(helpRequest);
        } else {
            throw "Unexpected message";
        }

        if (remainingResponsesNumber == 0 && (status.MPI_TAG == NURSE_RELEASE_NOTIFICATION || status.MPI_TAG == HELP_RESP)) {
            served = tryToGetHelp(); 
        }
    }
    releaseNurses();
}

bool Bum::tryToGetHelp() {
    unsigned int sumOfBumsWeights = 0;
    bool myRequestPositionFound = false;
    set<HelpRequest>::iterator it = helpRequests.begin();

    while (!myRequestPositionFound) {
        sumOfBumsWeights += (*it).weight;
        myRequestPositionFound = ((*it).processId == id);

        it++;
    }

    time++;
    return sumOfBumsWeights <= worldParameters->p;
}

void Bum::releaseNurses() {
    HelpRequest notifications[worldParameters->s - 1];
    int notificationsIterator = 0;

    for (unsigned int i = 0; i < worldParameters->s; i++) {
        if (museumAttendanceList[i] != id) {
            time++;

            notifications[notificationsIterator].processId = id;
            notifications[notificationsIterator].timestamp = myHelpRequest->timestamp;
            notifications[notificationsIterator].currentTime = time;
            notifications[notificationsIterator].weight = weight;

            MPI_Request status;
            MPI_Isend(&notifications[notificationsIterator], 1, MPIHelpRequest::getInstance().getType(), museumAttendanceList[i], 
                      NURSE_RELEASE_NOTIFICATION, MPI_COMM_WORLD, &status);
            MPI_Request_free(&status);

            notificationsIterator++;
        }
    }
}

void Bum::insertHelpRequest(HelpRequest &helpRequest) {
    if (helpRequestsFilter.find(helpRequest) == helpRequestsFilter.end()) {
        helpRequests.insert(helpRequest);
    }
}

void Bum::leaveMuseum() {
    if (museumAttendanceList[worldParameters->s - 1] == id) {
        waitForOthersToExit();
    }

    notifyAboutExit();
}

void Bum::notifyAboutExit() {
    Request notifications[worldParameters->m - 1];
    int notificationsIterator = 0;

    for (unsigned int i = 0; i < worldParameters->m; i++) {
        if (bumsIds[i] != id) {
            time++;

            notifications[notificationsIterator].processId = id;
            notifications[notificationsIterator].timestamp = myEnterRequest->timestamp;
            notifications[notificationsIterator].currentTime = time;

            MPI_Request status;
            MPI_Isend(&notifications[notificationsIterator], 1, MPIRequest::getInstance().getType(), bumsIds[i], 
                      EXIT_NOTIFICATION, MPI_COMM_WORLD, &status);
            MPI_Request_free(&status);

            notificationsIterator++;
        }
    }
}

void Bum::waitForOthersToExit() {
    bool canExit = false;
    
    while (!canExit) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == ENTER_REQ) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

            delayedEnterRequests.push_back(enterRequest);

        } else if (status.MPI_TAG == EXIT_NOTIFICATION) {
            Request exitNotification;
            MPI_Recv(&exitNotification, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > exitNotification.currentTime) ? time : exitNotification.currentTime) + 1;

            exitNotifications.push_back(exitNotification);

        } else if (status.MPI_TAG == HELP_REQ) {
            HelpRequest helpRequest;
            MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

            HelpRequest response(-1, -1, ++time, -1);
            MPI_Send(&response, 1, MPIHelpRequest::getInstance().getType(), helpRequest.processId, HELP_RESP, MPI_COMM_WORLD);

        } else if (status.MPI_TAG == NURSE_RELEASE_NOTIFICATION) {
            HelpRequest helpRequest;
            MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;
        } else {
            throw "Unexpected message";
        }
        canExit = (exitNotifications.size() == (worldParameters->m - 1));
    }
}
