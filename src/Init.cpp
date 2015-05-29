#include "../inc/Init.h"
#include <cstdlib>

int parse(Parameters &param, int argc, char * const argv[]) {
	int opt, count = 0;

	param.report = true;

	while((opt = getopt (argc, argv, "p:s:v")) != -1) {
		switch(opt) {
			case 'p':
				param.p = atoi(optarg);
				count++;
			break;

			case 's':
				param.s = atoi(optarg);
				count++;
			break;

			case 'v':
				param.report = false;
			break;
		}
	}

	if(count == 2) {
		return 0;
	} else { 
		return -1;
	}
}

int initialize(Parameters &param, int rank, int size) {
	//Set m parameter
	param.m = size - 1;

	//Check parameters correcntess
	if(param.m > param.s && 4*param.p < param.s) {
		//Create objects
		if(rank == 0) {

		} else {
			srand(rank);
			
		}
	} else if(param.m > param.s) {
		return -1;
	} else {
		return -2;
	}
}