//
// Created by alonakor@wincs.cs.bgu.ac.il on 07/01/2020.
//
#include <thread>
#include <mutex>
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include "Client.h"
#include "KeyBoardReader.h"
#include "SocketReader.h"

using namespace std;


int main() {

    //maps lockers for synchronize
    mutex inventoryMutex;
    mutex bookLenderMutex;;

    string inputString;
    vector<string> input;
    short port;
    string host;
    bool loggedin = false;
    cout << "enter login input" << endl;

    while (!loggedin) {
        getline(cin, inputString);
        istringstream s(inputString);
        do {
            string word;
            s >> word;
            input.push_back(word);
        } while (s);

        string hostPort = input[1];
        size_t colon = hostPort.find(":");
        host = hostPort.substr(0, colon);
        port = std::stoi(hostPort.substr(colon + 1));
        // port = boost::lexical_cast<short>(hostPort.substr(colon + 1));  //cast port from string to short


        if (input[0] == "login")
            loggedin = true;
        else cout << "Please login first" << endl;
    }

    ConnectionHandler connectionHandler(host, port);
    //ConnectionHandler *connectionHandler = new ConnectionHandler(host, port);

    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
    Client *client = new Client(host, input[2], input[3], connectionHandler);


    KeyBoardReader keyBoardReader(*client, inventoryMutex, bookLenderMutex);
    SocketReader socketReader(*client, inventoryMutex, bookLenderMutex);

    thread threadKeyBoard(&KeyBoardReader::runThreadKeyboard, &keyBoardReader);
    thread threadSocket(&SocketReader::runThreadSocket, &socketReader);


    threadKeyBoard.join();
    threadSocket.join();

    delete (client);
    input.clear();

    return 0;
}
