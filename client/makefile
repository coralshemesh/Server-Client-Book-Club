CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system

all: ClientMain
	g++ -pthread -o bin/StompBookClubClient bin/ConnectionHandler.o bin/Client.o bin/Stompframe.o bin/KeyBoardReader.o bin/SocketReader.o bin/ClientMain.o $(LDFLAGS)

ClientMain: bin/ConnectionHandler.o bin/ClientMain.o bin/Client.o bin/Stompframe.o bin/KeyBoardReader.o bin/SocketReader.o

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++	-pthread	$(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/ClientMain.o: src/ClientMain.cpp
	g++	-pthread	$(CFLAGS) -o bin/ClientMain.o src/ClientMain.cpp
	
bin/Client.o: src/Client.cpp
	g++	-pthread	$(CFLAGS) -o bin/Client.o src/Client.cpp
	
bin/Stompframe.o: src/Stompframe.cpp
	g++	-pthread	$(CFLAGS) -o bin/Stompframe.o src/Stompframe.cpp	
	
bin/KeyBoardReader.o: src/KeyBoardReader.cpp
	g++	-pthread	$(CFLAGS) -o bin/KeyBoardReader.o src/KeyBoardReader.cpp
	
bin/SocketReader.o: src/SocketReader.cpp
	g++	-pthread	$(CFLAGS) -o bin/SocketReader.o src/SocketReader.cpp
	
.PHONY: clean
clean:
	rm -f bin/*
