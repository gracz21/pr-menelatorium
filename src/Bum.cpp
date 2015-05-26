#include <mpi.h>

#include "../inc/Bum.h"
#include "../inc/messages.h"
#include "../inc/Request.h"

Bum::Bum(int id, unsigned short weight, const Parameters& worldParameters, int time) {
    this->id = id;
    this->time = time;
}

int Bum::getId() {
    return id;
}

void Bum::run() {
    hangAround();
}

void Bum::hangAround() {

}
