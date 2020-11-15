//
// Created by alonakor@wincs.cs.bgu.ac.il on 11/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_SOCKETREADER_H
#define BOOST_ECHO_CLIENT_SOCKETREADER_H

#include <string>
#include "ConnectionHandler.h"
#include "Client.h"
#include <mutex>


class SocketReader {

private:
    ConnectionHandler *connectionHandler;
    Client *client;
    bool logoutFlag;

    //maps lockers for synchronize
    std::mutex *inventoryMutex;
    std::mutex *bookLenderMutex;

public:

    SocketReader(Client&, std::mutex&,std::mutex&);
    ~SocketReader();
    SocketReader(const SocketReader&);
    SocketReader &operator=(const SocketReader&);

    void runThreadSocket();

    void addedMessage();

    void borrowMessage(std::string);

    void hasMessage(std::string, std::string,std::string);

    void takingMessage(std::string, std::string, std::string);

    void statusMessage(std::string);

    void returnMessage(std::string, std::string, std::string);

    Stompframe toStompframe(std::vector<std::string>);

    std::string getBookName(size_t,size_t,std::vector<std::string>*);

    int findLastWord(std::vector<std::string>*);


};


#endif //BOOST_ECHO_CLIENT_SOCKETREADER_H
