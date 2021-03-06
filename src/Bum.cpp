#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "../inc/Bum.h"
#include "../inc/messages.h"
#include "../inc/Request.h"
#include "../inc/MPIRequest.h"
#include "../inc/MPIHelpRequest.h"
#include "../inc/HangingAround.h"
#include "../inc/WaitingForEnterResponses.h"
#include "../inc/WaitingForAttendanceList.h"
#include "../inc/WaitingForHelp.h"
#include "../inc/WaitingForExit.h"

using namespace std;

Bum::Bum(int id, unsigned short weight, const Parameters *worldParameters, int *bumsIds, int time) {
    this->id = id;
    this->weight = weight;
    this->worldParameters = worldParameters;
    this->time = time;
    this->bumsIds = bumsIds;
    museumLocked = false;

    museumAttendanceList = new int[worldParameters->s];
    states["hanging_around"] = new HangingAround(this); 
    states["waiting_for_enter_responses"] = new WaitingForEnterResponses(this); 
    states["waiting_for_attendance_list"] = new WaitingForAttendanceList(this); 
    states["waiting_for_help"] = new WaitingForHelp(this); 
    states["waiting_for_exit"] = new WaitingForExit(this); 
}

Bum::~Bum() {
    delete [] museumAttendanceList;
    delete states["hanging_around"];
    delete states["waiting_for_enter_responses"];
    delete states["waiting_for_attendance_list"];
    delete states["waiting_for_help"];
    delete states["waiting_for_exit"];
}

void Bum::run() {
    while (true) {
        hangAround();
        goToMuseum();
        participateInExposition(); 
    }
}

void Bum::hangAround() {
    bool wantsToGoToMuseum = false;
    while (!wantsToGoToMuseum) {
        if(worldParameters->report)
            printf("Proces: %d, czas: %d - włóczę się po mieście\n", id, time);
        time++;

        int messageAvailable = 0;
        MPI_Status status;
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageAvailable, &status);

        if (messageAvailable) {
            if(worldParameters->report)
                printf("Proces: %d, czas: %d - włócząc się po mieście odebrałem wiadomość typu %d od %d\n", id, time, status.MPI_TAG, status.MPI_SOURCE);
            states["hanging_around"]->handleMessage(status);
        }

        wantsToGoToMuseum = ((rand() % 10) <= 4);
    }
}

void Bum::goToMuseum() {
    printf("Proces: %d, czas: %d - chcę wejść do muzeum\n", id, time);
    museumAttendanceListReceived = false;

    sendEnterRequests();
    waitForEnterResponses();
    waitForExpositionStart();
}

void Bum::sendEnterRequests() {
    enterRequests.clear();
    enterRequestsFilter.clear();
    enterRequests.insert(Request(id, time, time));
    myEnterRequest = &(*enterRequests.begin());

    Request requestsToSend[worldParameters->m - 1];
    int requestsIterator = 0;

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Rozsyłam żądanie wejścia do muzeum\n", id, time);
    for (unsigned int i = 0; i < worldParameters->m; i++) {
        if (bumsIds[i] != id) {
            requestsToSend[requestsIterator].processId = id;
            requestsToSend[requestsIterator].timestamp = myEnterRequest->timestamp;
            requestsToSend[requestsIterator].currentTime = ++time;

            MPI_Request status;
            MPI_Isend(&requestsToSend[requestsIterator], 1, MPIRequest::getInstance().getType(), bumsIds[i], ENTER_REQ, MPI_COMM_WORLD, &status);
            MPI_Request_free(&status);

            requestsIterator++;
        }
    }
}

void Bum::waitForEnterResponses() {
    int remainingResponses = worldParameters->m - 1;
    bool canEnter = (remainingResponses == 0);

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Czekam na potwierdzenia odebrania mojego żądania wejścia do muzeum\n", id, time);
    while (!canEnter) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        states["waiting_for_enter_responses"]->handleMessage(status);

        if (status.MPI_TAG == ENTER_RESP) {
            remainingResponses--;
        }

        if (!museumLocked && remainingResponses == 0 && (status.MPI_TAG == EXPO_START || status.MPI_TAG == EXIT_NOTIFICATION || status.MPI_TAG == ENTER_RESP)) {
            canEnter = tryToEnterMuseum();
        }
    }
    printf("Proces: %d, czas: %d - Mogę wejść do muzeum\n", id, time);
}

bool Bum::tryToEnterMuseum() {
    unsigned int myPosition = 0;

    for (set<Request>::iterator it = enterRequests.begin(); (*it).processId != id; it++, myPosition++);

    return myPosition <= (worldParameters->s - 1);
}

void Bum::waitForExpositionStart() {
    printf("Proces: %d, czas: %d - Czekam na rozpoczęcie ekspozycji\n", id, time);
    unsigned int myPosition = 0;

    for (set<Request>::iterator it = enterRequests.begin(); (*it).processId != id; it++, myPosition++);

    if (myPosition == worldParameters->s - 1) {
        sendAttendanceList(); 
    } else {
        waitForAttendanceList();
    }
    museumLocked = true;
}

void Bum::sendAttendanceList() {
    Request attendanceListWrapper[worldParameters->s];
    set<int> attendanceListLookup;

    unsigned int i = 0;
    for (set<Request>::iterator it = enterRequests.begin(); i < worldParameters->s; i++, it++) {
        museumAttendanceList[i] = (*it).processId;
        attendanceListLookup.insert((*it).processId);
        attendanceListWrapper[i] = *it;
    }

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Zakładam blokadę na muzeum\n", id, time);
    for (i = 0; i < worldParameters->m; i++) {
        if (attendanceListLookup.find(bumsIds[i]) == attendanceListLookup.end()) {
            Request lock(-1, -1, ++time);
            MPI_Send(&lock, 1, MPIRequest::getInstance().getType(),  bumsIds[i], MUSEUM_LOCK, MPI_COMM_WORLD);
        }
    }

    waitForLockConfirmations();
    sayHelloToMuseumService();

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Wysyłam listę obecności\n", id, time);
    for (i = 0; i < worldParameters->s; i++) {
        if (museumAttendanceList[i] != id) {
            attendanceListWrapper[worldParameters->s - 1].currentTime = ++time;
            MPI_Send(attendanceListWrapper, worldParameters->s, MPIRequest::getInstance().getType(), museumAttendanceList[i], EXPO_START, MPI_COMM_WORLD);
        }
    }
}

void Bum::waitForLockConfirmations() {
    unsigned int remainingConfirmations = worldParameters->m - worldParameters->s;

    while (remainingConfirmations > 0) {
        Request confirmation;
        MPI_Recv(&confirmation, 1, MPIRequest::getInstance().getType(), MPI_ANY_SOURCE, MUSEUM_LOCK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time = ((time > confirmation.currentTime) ? time : confirmation.currentTime) + 1;

        remainingConfirmations--;
    }
}

void Bum::sayHelloToMuseumService() {
    Request notification(-1, -1, ++time);

    MPI_Send(&notification, 1, MPIRequest::getInstance().getType(), 0, EXPO_START, MPI_COMM_WORLD);

    MPI_Recv(&notification, 1, MPIRequest::getInstance().getType(), 0, EXPO_START_CONFIRMATION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > notification.currentTime) ? time : notification.currentTime) + 1;
}

void Bum::waitForAttendanceList() {
    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Czekam na listę obecności\n", id, time);
    while (!museumAttendanceListReceived) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        states["waiting_for_attendance_list"]->handleMessage(status);
    }
}

void Bum::participateInExposition() {
    exitNotifications.clear();
    printf("Proces: %d, czas: %d - Jestem na ekspozycji\n", id, time);
    time++;

    bool gotDrunk = ((rand() % 10) <= 4);

    if (gotDrunk) {
        printf("Proces: %d, czas: %d - Upiłem się i czekam na pielęgniarzy\n", id, time);
        callForHelp(); 
        waitForHelp();
        printf("Proces: %d, czas: %d - Otrzymałem pomoc od pielęgniarzy\n", id, time);
    }

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
            helpRequestBuffers[helpRequestsIterator].processId = id;
            helpRequestBuffers[helpRequestsIterator].timestamp = myHelpRequest->timestamp;
            helpRequestBuffers[helpRequestsIterator].currentTime = ++time;
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

        states["waiting_for_help"]->handleMessage(status);

        if (status.MPI_TAG == HELP_RESP) {
            remainingResponsesNumber--;
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

    return sumOfBumsWeights <= worldParameters->p;
}

void Bum::releaseNurses() {
    HelpRequest notifications[worldParameters->s - 1];
    int notificationsIterator = 0;

    for (unsigned int i = 0; i < worldParameters->s; i++) {
        if (museumAttendanceList[i] != id) {
            notifications[notificationsIterator].processId = id;
            notifications[notificationsIterator].timestamp = myHelpRequest->timestamp;
            notifications[notificationsIterator].currentTime = ++time;
            notifications[notificationsIterator].weight = weight;

            MPI_Request status;
            MPI_Isend(&notifications[notificationsIterator], 1, MPIHelpRequest::getInstance().getType(), museumAttendanceList[i], 
                      NURSE_RELEASE_NOTIFICATION, MPI_COMM_WORLD, &status);
            MPI_Request_free(&status);

            notificationsIterator++;
        }
    }
}

void Bum::leaveMuseum() {
    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Zamierzam opuścić muzeum\n", id, time);
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
    MPI_Isend(&exitNotification, 1, MPIRequest::getInstance().getType(), museumAttendanceList[worldParameters->s - 1], SINGLE_EXIT_NOTIFICATION, MPI_COMM_WORLD, &status);
    MPI_Request_free(&status);
}

void Bum::waitForOthersToExit() {
    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Czekam na innych aż wyjdą\n", id, time);
    bool canExit = (worldParameters->s == 1);

    while (!canExit) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        states["waiting_for_exit"]->handleMessage(status);

        canExit = (exitNotifications.size() == (worldParameters->s - 1));
    }

    sayGoodbyeToMuseumService();

    printf("Proces: %d, czas: %d - Informuję o opuszczeniu muzeum\n", id, time);
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
    printf("Proces: %d, czas: %d - Poinformowałem resztę o opuszczeniu muzeum\n", id, time);
    museumLocked = false;
}

void Bum::sayGoodbyeToMuseumService() {
    Request notification(-1, -1, ++time);
    MPI_Send(&notification, 1, MPIRequest::getInstance().getType(), 0, SINGLE_EXIT_NOTIFICATION, MPI_COMM_WORLD);
}

void Bum::addToEnterRequestsFilter(Request *requestsToFiler) {
    for (unsigned int i = 0; i < worldParameters->s; i++) {
        enterRequestsFilter.insert(requestsToFiler[i]);
    }
}

void Bum::removeFromEnterRequests(Request *requestsToRemove) {
    for (unsigned int i = 0; i < worldParameters->s; i++) {
        enterRequests.erase(requestsToRemove[i]);
    }
}

void Bum::answerDontWantToEnterMuseum(MPI_Status &status) {
    Request enterRequest;
    MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

    Request response(-1, -1, ++time);
    MPI_Send(&response, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, ENTER_RESP, MPI_COMM_WORLD);

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Mówię procesowi %d, że nie chcę wejść do muzeum\n", id, time, status.MPI_SOURCE);
}

void Bum::answerWantToEnterMuseum(MPI_Status &status) {
    Request enterRequest;
    MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

    insertEnterRequest(enterRequest); 

    Request response = *myEnterRequest;
    response.currentTime = ++time;
    MPI_Send(&response, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, ENTER_RESP, MPI_COMM_WORLD);

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Mówię procesowi %d, że chcę wejść do muzeum\n", id, time, status.MPI_SOURCE);
}

void Bum::ignoreExitNotifications(MPI_Status &status) {
    Request exitNotifications[worldParameters->s];
    MPI_Recv(exitNotifications, worldParameters->s, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > exitNotifications[worldParameters->s - 1].currentTime) ? time : exitNotifications[worldParameters->s - 1].currentTime) + 1;

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Otrzymałem informację od %d, że mogę zdjąć blokadę\n", id, time, status.MPI_SOURCE);

    museumLocked = false;
}

void Bum::answerDontNeedNurse(MPI_Status &status) {
    HelpRequest helpRequest;
    MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

    HelpRequest response(-1, -1, ++time, -1);
    MPI_Send(&response, 1, MPIHelpRequest::getInstance().getType(), helpRequest.processId, HELP_RESP, MPI_COMM_WORLD);
}

void Bum::ignoreNurseReleaseNotification(MPI_Status &status) {
    HelpRequest helpRequest;
    MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;
}

void Bum::saveEnterRequest(MPI_Status &status) {
    Request enterRequest;
    MPI_Recv(&enterRequest, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > enterRequest.currentTime) ? time : enterRequest.currentTime) + 1;

    if (enterRequest.processId != -1) {
        insertEnterRequest(enterRequest);  
        if(worldParameters->report)
            printf("Proces: %d, czas: %d - Otrzymałem żądanie wejścia do muzeum od %d\n", id, time, status.MPI_SOURCE);
    } else {
        if(worldParameters->report)
            printf("Proces: %d, czas: %d - %d nie chce wejść do muzeum\n", id, time, status.MPI_SOURCE);
    }
}

void Bum::saveExitNotification(MPI_Status &status) {
    Request exitNotifications[worldParameters->s];
    MPI_Recv(exitNotifications, worldParameters->s, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > exitNotifications[worldParameters->s - 1].currentTime) ? time : exitNotifications[worldParameters->s -1].currentTime) + 1;

    addToEnterRequestsFilter(exitNotifications);
    removeFromEnterRequests(exitNotifications);

    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Proces %d mówi, że mogę zdjąć blokadę\n", id, time, status.MPI_SOURCE);
    museumLocked = false;
}

void Bum::saveMuseumAttendanceList(MPI_Status &status) {
    Request packedAttendanceList[worldParameters->s];

    MPI_Recv(packedAttendanceList, worldParameters->s, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > packedAttendanceList[worldParameters->s - 1].currentTime) ? time : packedAttendanceList[worldParameters->s - 1].currentTime) + 1;

    for (unsigned int i = 0; i < worldParameters->s; i++) {
        museumAttendanceList[i] = packedAttendanceList[i].processId;
    }
    if(worldParameters->report)
        printf("Proces: %d, czas: %d - otrzymałem listę obecności od %d\n", id, time, status.MPI_SOURCE);
    museumAttendanceListReceived = true;
}

void Bum::saveHelpReq(MPI_Status &status) {
    HelpRequest helpRequest;
    MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

    insertHelpRequest(helpRequest);
    HelpRequest response = *myHelpRequest;
    response.currentTime = ++time;
    MPI_Send(&response, 1, MPIHelpRequest::getInstance().getType(), helpRequest.processId, HELP_RESP, MPI_COMM_WORLD);
}

void Bum::saveHelpResp(MPI_Status &status) {
    HelpRequest helpRequest;
    MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

    if (helpRequest.processId != -1) {
        insertHelpRequest(helpRequest);
    }
}

void Bum::saveNurseRelease(MPI_Status &status) {
    HelpRequest helpRequest;
    MPI_Recv(&helpRequest, 1, MPIHelpRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > helpRequest.currentTime) ? time : helpRequest.currentTime) + 1;

    helpRequestsFilter.insert(helpRequest);
    helpRequests.erase(helpRequest);
}

void Bum::delayExitNotification(MPI_Status &status) {
    Request exitNotification;
    MPI_Recv(&exitNotification, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > exitNotification.currentTime) ? time : exitNotification.currentTime) + 1;

    exitNotifications.push_back(exitNotification);
    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Proces %d informuje mnie, że chce wyjść\n", id, time, status.MPI_SOURCE);
}

void Bum::saveMuseumLock(MPI_Status &status) {
    Request lock;
    MPI_Recv(&lock, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    time = ((time > lock.currentTime) ? time : lock.currentTime) + 1;

    museumLocked = true;

    lock.currentTime = ++time;
    MPI_Send(&lock, 1, MPIRequest::getInstance().getType(), status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD);
    if(worldParameters->report)
        printf("Proces: %d, czas: %d - Proces %d każe założyć blokadę muzeum, odsyłam potwierdzenie\n", id, time, status.MPI_SOURCE);
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
