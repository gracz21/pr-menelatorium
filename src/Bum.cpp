#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

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
    emptyDelayedEnterRequests();
    hangAround();
    goToMuseum();
    participateInExposition();
}

void Bum::emptyDelayedEnterRequests() {
    while (!delayedEnterRequests.empty()) {
        Request enterRequest = delayedEnterRequests.front();
        Request response = Request(-1, -1, ++time);

        MPI_Send(&response, 1, MPIRequest::getInstance().getType(), enterRequest.processId, ENTER_RESP, MPI_COMM_WORLD);
        
        delayedEnterRequests.pop_front();
    }
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
        Request exitNotifications[worldParameters->s];
        MPI_Recv(exitNotifications, worldParameters->s, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time = ((time > exitNotifications[worldParameters->s - 1].currentTime) ? time : exitNotifications[worldParameters->s - 1].currentTime) + 1;

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
    cout << "Proces: " << id << ", czas: " << time << " chcę wejść do muzeum" << endl;
    sendEnterRequests();
    waitForEnterResponses();
    waitForExpositionStart();
}

void Bum::sendEnterRequests() {
    enterRequests.clear();
    enterRequestsFilter.clear();
    enterRequests.insert(Request(id, time, time));
    myEnterRequest = &(*enterRequests.begin());

    Request enterRequests[worldParameters->m - 1];
    int requestsIterator = 0;

    for (unsigned int i = 0; i < worldParameters->m; i++) {
        if (bumsIds[i] != id) {
            time++;

            enterRequests[requestsIterator].processId = id;
            enterRequests[requestsIterator].timestamp = myEnterRequest->timestamp;
            enterRequests[requestsIterator].currentTime = time;

            cout << "Proces: " << id << ", czas: " << time << " pytam proces " << bumsIds[i] << " czy chce wejść do muzeum" << endl;
            MPI_Request status;
            MPI_Isend(&enterRequests[requestsIterator], 1, MPIRequest::getInstance().getType(), bumsIds[i], ENTER_REQ, MPI_COMM_WORLD, &status);
            MPI_Request_free(&status);
            cout << "Proces: " << id << ", czas: " << time << " zapytałem proces " << bumsIds[i] << endl;

            requestsIterator++;
        }
    }
    cout << "Proces: " << id << " zapytałem wszystkich" << endl;
}

void Bum::waitForEnterResponses() {
    int remainingResponses = worldParameters->m - 1;
    bool canEnter = (remainingResponses == 0);

    cout << "Proces: " << id << " czekam na " << remainingResponses << " odpowiedzi" << endl;

    while (!canEnter) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        cout << "Proces: " << id << " Dostałem wiadomość od " << status.MPI_SOURCE << " typu " << status.MPI_TAG << endl;

        if (status.MPI_TAG == ENTER_REQ) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

            insertEnterRequest(enterRequest);  
            Request response = *myEnterRequest;
            response.currentTime = time;

            MPI_Send(&response, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, ENTER_RESP, MPI_COMM_WORLD);

        } else if (status.MPI_TAG == ENTER_RESP) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

            if (enterRequest.processId != -1) {
                insertEnterRequest(enterRequest);  
            }
            remainingResponses--;

        } else if (status.MPI_TAG == EXIT_NOTIFICATION) {
            Request exitNotifications[worldParameters->s];
            MPI_Recv(exitNotifications, worldParameters->s, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > exitNotifications[worldParameters->s - 1].currentTime) ? time : exitNotifications[worldParameters->s -1].currentTime) + 1;

            addToEnterRequestsFilter(exitNotifications);
            removeFromEnterRequests(exitNotifications);

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

        if (remainingResponses == 0 && (status.MPI_TAG == EXIT_NOTIFICATION || status.MPI_TAG == ENTER_RESP)) {
            canEnter = tryToEnterMuseum();
        }
    }
}

bool Bum::tryToEnterMuseum() {
    unsigned int myPosition = 0;

    cout << "Proces " << id << " próbuję wejść do muzeum" << endl;
    for (set<Request>::iterator it = enterRequests.begin(); (*it).processId != id; it++) {
        myPosition++;
    }

    return myPosition <= (worldParameters->s - 1);
}

void Bum::waitForExpositionStart() {
    cout << "Proces: " << id << " oczekiwanie na listę obecności" << endl;
    
    unsigned int myPosition = 0;
    for (set<Request>::iterator it = enterRequests.begin(); (*it).processId != id; it++, myPosition++);
    if (myPosition == worldParameters->s - 1) {
        sendAttendanceList(); 
    } else {
        waitForAttendanceList();
    }
}

void Bum::sendAttendanceList() {
    Request attendanceListWrapper[worldParameters->s];
    unsigned int i = 0;

    for (set<Request>::iterator it = enterRequests.begin(); i < worldParameters->s; i++) {
        museumAttendanceList[i] = (*it).processId;
        attendanceListWrapper[i] = *it;
    }

    for (i = 0; i < worldParameters->s; i++) {
        if (museumAttendanceList[i] != id) {
            attendanceListWrapper[worldParameters->s - 1].currentTime = ++time;
            MPI_Send(attendanceListWrapper, worldParameters->s, MPIRequest::getInstance().getType(), museumAttendanceList[i], EXPO_START, MPI_COMM_WORLD);
        }
    }
}

void Bum::waitForAttendanceList() {
    bool museumAttendanceListUpdated = false;

    while (!museumAttendanceListUpdated) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        cout << "Proces: " << id << " Dostałem wiadomość od " << status.MPI_SOURCE << " typu " << status.MPI_TAG << endl;

        if (status.MPI_TAG == ENTER_REQ) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

            Request response = *myEnterRequest;
            response.currentTime = time;

            MPI_Send(&response, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, ENTER_RESP, MPI_COMM_WORLD);

        } else if (status.MPI_TAG == EXPO_START) {
            Request packedAttendanceList[worldParameters->s];

            MPI_Recv(packedAttendanceList, worldParameters->s, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > packedAttendanceList[worldParameters->s - 1].currentTime) ? time : packedAttendanceList[worldParameters->s - 1].currentTime) + 1;

            for (unsigned int i = 0; i < worldParameters->s; i++) {
                museumAttendanceList[i] = packedAttendanceList[i].processId;
            }
            museumAttendanceListUpdated = true;

        } else {
            throw "Unexpected message";
        }
    }
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

    HelpRequest helpRequestBuffers[worldParameters->s - 1];
    int helpRequestsIterator = 0;

    for (unsigned int i = 0; i < worldParameters->s; i++) {
        if (museumAttendanceList[i] != id) {
            time++;

            helpRequestBuffers[helpRequestsIterator].processId = id;
            helpRequestBuffers[helpRequestsIterator].timestamp = myHelpRequest->timestamp;
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
    bool served = (remainingResponsesNumber == 0);
    
    while (!served) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == ENTER_REQ) {
            Request enterRequest;
            MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

            delayedEnterRequests.push_back(enterRequest);

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

void Bum::insertEnterRequest(Request &enterRequest) {
    if (enterRequestsFilter.find(enterRequest) == enterRequestsFilter.end()) {
        enterRequests.insert(enterRequest);
    }
}

void Bum::leaveMuseum() {
    if (museumAttendanceList[worldParameters->s - 1] == id) {
        waitForOthersToExit();
    } else {
        notifyAboutExit();
    }

}

void Bum::notifyAboutExit() {
    Request exitNotification = *myEnterRequest;
    exitNotification.currentTime = ++time;

    MPI_Request status;
    MPI_Isend(&exitNotification, 1, MPIRequest::getInstance().getType(), museumAttendanceList[worldParameters->s - 1], EXIT_NOTIFICATION, MPI_COMM_WORLD, &status);
    MPI_Request_free(&status);
}

void Bum::waitForOthersToExit() {
    bool canExit = (worldParameters->s == 1);
    
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

    Request notificationsToSend[worldParameters->s];
    unsigned int i = 0;
    for (list<Request>::iterator it = exitNotifications.begin(); it != exitNotifications.end(); it++, i++) {
        notificationsToSend[i] = *it;
    }
    notificationsToSend[worldParameters->s - 1] = *myEnterRequest;
    for (i = 0; i < worldParameters->m; i++) {
        if (bumsIds[i] != id) {
            notificationsToSend[worldParameters->s - 1].currentTime = ++time;

            MPI_Send(notificationsToSend, worldParameters->s, MPIRequest::getInstance().getType(), bumsIds[i], EXIT_NOTIFICATION,  MPI_COMM_WORLD);
        }
    }
}

void Bum::addToEnterRequestsFilter(Request *enterRequests) {
    for (unsigned int i = 0; i < worldParameters->s; i++) {
        enterRequestsFilter.insert(enterRequests[i]);
    }
}

void Bum::removeFromEnterRequests(Request *enterRequests) {
    for (unsigned int i = 0; i < worldParameters->s; i++) {
        this->enterRequests.erase(enterRequests[i]);
    }
}
