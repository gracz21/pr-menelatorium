#include "../inc/ArgParsing.h"
#include <cstdlib> 

int parse(Parameters &param, int argc, char * const argv[]) {
	int opt;
	if(argc > 4) {
		while((opt = getopt (argc, argv, "m:p:s:")) != -1) {
			switch(opt) {
				case 'm':
					param.m = atoi(optarg);
				break;
				case 'p':
					param.p = atoi(optarg);
				break;
				case 's':
					param.s = atoi(optarg);
				break;
				default:
				break;
			}
		}
		return 0;
	} else return -1;
}