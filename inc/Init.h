#ifndef INIT_H
#define INIT_H

#include <unistd.h>
#include "Parameters.h"

/*
* Parse arguments from console.
* Return 0 if 2 expected parameters (p, s) was given, -1 otherwise.
*/
int parse(Parameters &param, int argc, char * const argv[]);
/*
* Check parameters correctness and initialize objects (n - 1 process create bum object,
* first process create museum service object)
* Return 0 if parameters was correct and objects was initialized. 
* Return negativ value (-1 or -2) if parameters wasn't correct.
*/
int initialize(int rank, int size);

#endif