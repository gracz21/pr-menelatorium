#include <mpi.h>
#include <iostream>

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
}

void Bum::hangAround() {
    bool wantsToGoToMuseum = false;

    while (!wantsToGoToMuseum) {
        time++;
        wantsToGoToMuseum = ((rand() % 10) <= 4);
    }
}
