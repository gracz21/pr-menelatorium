#include "../inc/Init.h"
#include <cstdlib>

using namespace std;

int checkParameters(Parameters params) {
	if(params.m > params.s) {
		if(4*params.p < params.s) {
			return 0;
		} else {
			return -2;
		}
	} else {
		return -1;
	}	
}

int parseParameters(Parameters &params, int argc, char * const argv[], int size) {
	int opt, count = 0;

	params.report = true;
	params.m = size - 1;

	while((opt = getopt (argc, argv, "p:s:v")) != -1) {
		switch(opt) {
			case 'p':
				params.p = atoi(optarg);
				count++;
			break;

			case 's':
				params.s = atoi(optarg);
				count++;
			break;

			case 'v':
				params.report = false;
			break;
		}
	}

	if(count == 2) {
		if(checkParameters(params) == 0) {
			return 0;
		} else {
			return -2;
		}
	} else { 
		return -1;
	}
}

int randWeight(int rank) {
	srand(rank);
	return (rand() % 3) + 1; 
}

int* createBumsIds(int lastBumId) {
	int *bumsIds = new int[lastBumId];
	for(int i = 0; i < lastBumId; i++) {
		bumsIds[i] = i + 1;
	}

	return bumsIds;
} 