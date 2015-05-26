#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "../inc/Bum.h"
#include "../inc/messages.h"
#include "../inc/Request.h"

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
        switch (status.MPI_TAG) {
            case ENTER_REQ:

                break;
            case EXIT_NOTIFICATION:

                break;
            default:
                break;
        }
    }
}

void Bum::goToMuseum() {

}
