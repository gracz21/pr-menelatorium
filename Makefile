all: menele

menele: obj/ArgParsing.o obj/Bum.o obj/BumFactory.o obj/HelpRequest.o obj/main.o obj/MPICustomType.o obj/MPIHelpRequest.o obj/MPIRequest.o obj/Request.o
	mpicxx obj/ArgParsing.o obj/Bum.o obj/BumFactory.o obj/HelpRequest.o obj/main.o obj/MPICustomType.o obj/MPIHelpRequest.o obj/MPIRequest.o obj/Request.o -Wall -o menele
obj/ArgParsing.o: src/ArgParsing.cpp src/../inc/ArgParsing.h src/../inc/Parameters.h
	mpicxx src/ArgParsing.cpp -o $@ -Wall -c
obj/Bum.o: src/Bum.cpp src/../inc/Bum.h src/../inc/Request.h src/../inc/HelpRequest.h src/../inc/Parameters.h src/../inc/messages.h src/../inc/Request.h src/../inc/MPIRequest.h src/../inc/MPICustomType.h src/../inc/MPIHelpRequest.h
	mpicxx src/Bum.cpp -o $@ -Wall -c
obj/BumFactory.o: src/BumFactory.cpp src/../inc/BumFactory.h src/../inc/Bum.h src/../inc/Request.h src/../inc/HelpRequest.h src/../inc/Parameters.h
	mpicxx src/BumFactory.cpp -o $@ -Wall -c
obj/HelpRequest.o: src/HelpRequest.cpp src/../inc/HelpRequest.h
	mpicxx src/HelpRequest.cpp -o $@ -Wall -c
obj/main.o: src/main.cpp src/../inc/HelpRequest.h src/../inc/MPIRequest.h src/../inc/MPICustomType.h
	mpicxx src/main.cpp -o $@ -Wall -c
obj/MPICustomType.o: src/MPICustomType.cpp src/../inc/MPICustomType.h
	mpicxx src/MPICustomType.cpp -o $@ -Wall -c
obj/MPIHelpRequest.o: src/MPIHelpRequest.cpp src/../inc/MPIHelpRequest.h src/../inc/MPICustomType.h src/../inc/HelpRequest.h
	mpicxx src/MPIHelpRequest.cpp -o $@ -Wall -c
obj/MPIRequest.o: src/MPIRequest.cpp src/../inc/MPIRequest.h src/../inc/MPICustomType.h src/../inc/Request.h
	mpicxx src/MPIRequest.cpp -o $@ -Wall -c
obj/Request.o: src/Request.cpp src/../inc/Request.h
	mpicxx src/Request.cpp -o $@ -Wall -c
clean:
	rm -f obj/*
