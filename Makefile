CXX=mpicxx
CFLAGS=-Wall -c

all: menele

menele: main.cpp obj/Request.o obj/Menel.o obj/MPIRequest.o
	$(CXX) -Wall main.cpp obj/Request.o obj/Menel.o obj/MPIRequest.o -o menele

obj/Request.o: inc/Request.h src/Request.cpp
	$(CXX) $(CFLAGS) src/Request.cpp -o obj/Request.o

obj/Menel.o: inc/Menel.h src/Menel.cpp
	$(CXX) $(CFLAGS) src/Menel.cpp -o obj/Menel.o

obj/MPIRequest.o: inc/MPIRequest.h src/MPIRequest.cpp
	$(CXX) $(CFLAGS) src/MPIRequest.cpp -o obj/MPIRequest.o
