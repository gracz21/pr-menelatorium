all: menele
menele: obj/Bum.o obj/BumFactory.o obj/main.o obj/MPIRequest.o obj/Request.o
	mpicxx obj/Bum.o obj/BumFactory.o obj/main.o obj/MPIRequest.o obj/Request.o -Wall -o menele
obj/Bum.o: src/Bum.cpp src/../inc/Bum.h src/../inc/Request.h src/../inc/Message.h src/../inc/HelpRequest.h src/../inc/Parameters.h src/../inc/messages.h src/../inc/Request.h
	mpicxx src/Bum.cpp -o $@ -Wall -c
obj/BumFactory.o: src/BumFactory.cpp src/../inc/BumFactory.h src/../inc/Bum.h src/../inc/Request.h src/../inc/Message.h src/../inc/HelpRequest.h src/../inc/Parameters.h
	mpicxx src/BumFactory.cpp -o $@ -Wall -c
obj/main.o: src/main.cpp
	mpicxx src/main.cpp -o $@ -Wall -c
obj/MPIRequest.o: src/MPIRequest.cpp src/../inc/MPIRequest.h src/../inc/Request.h src/../inc/Message.h
	mpicxx src/MPIRequest.cpp -o $@ -Wall -c
obj/Request.o: src/Request.cpp src/../inc/Request.h src/../inc/Message.h
	mpicxx src/Request.cpp -o $@ -Wall -c
clean:
	rm -f obj/*
