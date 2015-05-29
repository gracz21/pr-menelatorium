CXX=mpicxx
CFLAGS=-Wall -c

all: menele

menele: main.cpp obj/Request.o obj/Bum.o obj/MPIRequest.o obj/BumFactory.o obj/Init.o
	$(CXX) -Wall main.cpp obj/Request.o obj/Bum.o obj/MPIRequest.o obj/Init.o -o menele

obj/Request.o: src/Request.cpp inc/Request.h 
	$(CXX) $(CFLAGS) src/Request.cpp -o obj/Request.o

obj/Bum.o: src/Bum.cpp inc/Bum.h inc/Request.h inc/Parameters.h
	$(CXX) $(CFLAGS) src/Bum.cpp -o obj/Bum.o

obj/MPIRequest.o: src/MPIRequest.cpp inc/MPIRequest.h 
	$(CXX) $(CFLAGS) src/MPIRequest.cpp -o obj/MPIRequest.o

obj/BumFactory.o: src/BumFactory.cpp inc/BumFactory.h inc/Bum.h
	$(CXX) $(CFLAGS) src/BumFactory.cpp -o obj/BumFactory.o

obj/Init.o: src/Init.cpp inc/Init.h inc/Parameters.h
	$(CXX) $(CFLAGS) src/Init.cpp -o obj/Init.o
