#include "ArgParsing.h"

void parse(Parameters param, int argc, char * const argv[]) {
	int opt;
	while((opt = = getopt (argc, argv, "m:p:s:")) {
		switch(opt) {
			case 'm':
				param.m = optarg;
			break;
			case 'p' :
				param.p = optarg;
			break;
			case 's' :
				param.s = optarg;
			break;
		}
	}
}