//
// Created by alonakor@wincs.cs.bgu.ac.il on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_KEYBOARDREADER_H
#define BOOST_ECHO_CLIENT_KEYBOARDREADER_H

#include <string>
#include <vector>
#include "Stompframe.h"
#include <map>
#include <mutex>
#include "Client.h"
#include "ConnectionHandler.h"

class KeyBoardReader {

private:
    bool exitFlag;
    std::vector<std::string> input;
    ConnectionHandler *connectionHandler;
    int subId;
    Client *client;

    //maps lockers for synchronize
    std::mutex *inventoryMutex;
    std::mutex *bookLenderMutex;

public:

    KeyBoardReader(Client&, std::mutex&,std::mutex&);
    KeyBoardReader(const KeyBoardReader&);
    KeyBoardReader &operator=(const KeyBoardReader&);
    ~KeyBoardReader();
    void runThreadKeyboard();
    void login();

    void join(std::string);

    void exit(std::string);

    void addBook(std::string, std::string);

    void borrowBook(std::string, std::string);

    void returnBook(std::string, std::string);

    void bookStatus(std::string);

    void logout();

};


#endif //BOOST_ECHO_CLIENT_KEYBOARDREADER_H
