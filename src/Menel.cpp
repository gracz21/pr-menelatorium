#include <mpi.h>

#include "../inc/Menel.h"
#include "../inc/messages.h"
#include "../inc/Request.h"

Menel::Menel(int id, int time) {
    this->id = id;
    this->time = time;
}

int Menel::getId() {
    return id;
}

void Menel::run() {
    hangAround();
}

void Menel::hangAround() {

}
