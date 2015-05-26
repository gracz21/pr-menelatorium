#include <mpi.h>

#include "../inc/Bum.h"
#include "../inc/messages.h"
#include "../inc/Request.h"

Bum::Bum(int id, Parameters param, int time) {
    this->id = id;
    this->time = time;
    this->param = param;
}

int Bum::getId() {
    return id;
}

void Bum::run() {
    hangAround();
}

void Bum::hangAround() {

}
