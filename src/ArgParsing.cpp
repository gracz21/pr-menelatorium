#include "../inc/ArgParsing.h"
#include <cstdlib> 

int parse(Parameters &param, int argc, char * const argv[]) {
	int opt, count = 0;

	param.report = true;

	while((opt = getopt (argc, argv, "m:p:s:v")) != -1) {
		switch(opt) {
			case 'm':
				param.m = atoi(optarg);
				count++;
			break;

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

	if(count >= 3)
		return 0;
	else return -1;
}