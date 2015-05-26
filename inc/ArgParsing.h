#ifndef ARGPARSING_H
#define ARGPARSING_H

#include <unistd.h>
#include "Parameters.h"

/*
* Parse arguments from console.
* Return 0 if 3 expected parameters (m, p, s) was given, -1 otherwise.
*/
int parse(Parameters &param, int argc, char * const argv[]);

#endif