//
// Created by alonakor@wincs.cs.bgu.ac.il on 04/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_CLIENT_H
#define BOOST_ECHO_CLIENT_CLIENT_H

#include <boost/asio.hpp>
#include <vector>
#include "ConnectionHandler.h"
#include <list>
#include <map>
#include "Stompframe.h"


using boost::asio::ip::tcp;

class Client {

private:
    std::string host;
    std::string name;
    std::string password;
    std::vector<std::string> fromServerVec;
    ConnectionHandler *connectionHandler;
    std::map<std::string, std::list<std::string> *> inventory; // topic : a list of book names
    std::map<std::string, std::string> bookLender; //book name : lender name
    std::vector<std::string> askedBooks;
    int receiptId;
    bool login;
    std::map<int, std::string> receiptNprint;
    std::map<std::string, int> topicsIdSub; // topic : subscription id


public:
    Client(std::string,std::string,std::string,ConnectionHandler&);
    ~Client();
    Client &operator=(const Client&);
    Client(const Client&);
    std::map<std::string, std::list<std::string> *>* getInventory();
    std::string getName();
    std::vector<std::string>* getAskedBooks();
    std::string getPassword();
    int getReceiptId();

    void incrementReceiptId(){ receiptId++;}

    void setAndAdd(int, std::string);
    void addToInventory(std::string,std::string);
    void addToBookLender(std::string,std::string);
    std::string createStringInventory();
    bool loggedIn();
    void setLoggedin(bool);
    ConnectionHandler* getCon();
    std::string receiptHandle(std::string);
    void addToAskedBooks(std::string);
    void removeFromInventory(std::string,std::string);
    std::string lenderName(std::string);
    bool inventoryContains(std::string, std::string);
    void addToTopicsIdSub(std::string, int idSub);
    int returnIdSub(std::string);
    bool findTopicsIdSub(std::string);
    void clearTopicsIdSub();
    std::string getHost();
    void removeFromBookLender(std::string);


};


#endif //BOOST_ECHO_CLIENT_CLIENT_H
