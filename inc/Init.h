#ifndef INIT_H
#define INIT_H

#include <unistd.h>
#include "Parameters.h"

int parseParameters(Parameters &param, int argc, char * const argv[], int size);
int randWeight(int rank);
int* createBumsIds(int lastBumId);

#endif