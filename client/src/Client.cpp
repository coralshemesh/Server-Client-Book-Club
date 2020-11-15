//
// Created by alonakor@wincs.cs.bgu.ac.il on 04/01/2020.
//

#include "Client.h"
#include <vector>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>


using boost::asio::ip::tcp;
using namespace std;

//constructor
Client::Client(string host, string username, string password, ConnectionHandler &connectionHandler1)
        : host(host), name(username), password(password), fromServerVec(), connectionHandler(&connectionHandler1),
          inventory(), bookLender(), askedBooks(), receiptId(99), login(false), receiptNprint(), topicsIdSub() {
}

//destructor
Client::~Client() {
    fromServerVec.clear();
    connectionHandler = nullptr;
    for (auto &it: inventory) {
        it.second->clear();
        delete (it.second);
    }
    inventory.clear();
    bookLender.clear();
    askedBooks.clear();
    receiptNprint.clear();

}

std::map<std::string, std::list<std::string> *> *Client::getInventory() {
    return &inventory;
}

std::string Client::getName() {
    return this->name;
}

std::string Client::getPassword() {
    return this->password;
}

std::vector<std::string> *Client::getAskedBooks() {
    return &askedBooks;
}

//add the event with its receipt id
void Client::setAndAdd(int id, string toPrint) {
    receiptNprint[id] = toPrint;
}

int Client::getReceiptId() {
    return receiptId;
}

void Client::addToInventory(std::string left, std::string right) {
    if (getInventory()->find(left) != getInventory()->end())
        getInventory()->find(left)->second->push_back(right);
    else {
        list<string> *l = new list<string>;
        l->push_back(right);
        inventory[left] = l;
    }
}


void Client::addToBookLender(string book, string owner) {
    bookLender[book] = owner;
}

std::string Client::createStringInventory() {
    string body;
    for (auto topic: inventory) {
        for (auto name : *topic.second) {
            body += name + ",";
        }
    }
    return body;
}

bool Client::loggedIn() {
    return login;
}

void Client::setLoggedin(bool flag) {
    this->login = flag;
}

ConnectionHandler *Client::getCon() {
    return connectionHandler;
}

string Client::receiptHandle(string receipt) {
    string print = receiptNprint.find(stoi(receipt))->second;
    //   cout << print << endl;
    return print.substr(0, print.find(' '));
}

void Client::addToAskedBooks(string book) {
    askedBooks.push_back(book);

}

void Client::removeFromInventory(string genre, string book) {
    inventory.find(genre)->second->remove(book);
}

string Client::lenderName(string book) {
    return bookLender.find(book)->second;
}

//check if the book exists in the clients' inventory
bool Client::inventoryContains(string genre, string bookName) {
    if (inventory.find(genre) != inventory.end()) {
        for (auto name : *inventory.find(genre)->second) {
            if (name == bookName) return true;
        }
    }
    return false;
}

//connects the user's subscription id to a topic
void Client::addToTopicsIdSub(string topic, int idSub) {
    topicsIdSub[topic] = idSub;
}

int Client::returnIdSub(string topic) {
    return topicsIdSub.find(topic)->second;
}

bool Client::findTopicsIdSub(string topic) {
    if (topicsIdSub.find(topic) != topicsIdSub.end()) return true;
    return false;
}

void Client::clearTopicsIdSub() {
    topicsIdSub.clear();
}

std::string Client::getHost() {
    return host;
}

void Client::removeFromBookLender(string name) {
    bookLender.erase(name);
    // bookLender.erase(find(bookLender.begin(), bookLender.end(), name));
}

//copy constructor
Client::Client(const Client &client)
        : host(), name(), password(), fromServerVec(), connectionHandler(), inventory(), bookLender(), askedBooks(),
          receiptId(), login(), receiptNprint(), topicsIdSub() {
    if (this != &client) {
        this->host = client.host;
        this->name = client.name;
        this->password = client.password;
        this->login = client.login;
        connectionHandler = client.connectionHandler;
        //connectionHandler = new ConnectionHandler(client.connectionHandler->getHost(),client.connectionHandler->getPort());
        for (auto topic: client.inventory) {
            list<string> *l = new list<string>;
            for (auto book : *topic.second) {
                l->push_back(book);
            }
            inventory[topic.first] = l;
        }
        for (auto book: client.bookLender) {
            bookLender[book.first] = book.second;
        }
        for (size_t i = 0; i < client.askedBooks.size(); i++) {
            askedBooks.push_back(client.askedBooks[i]);
        }
        for (auto receipt: client.receiptNprint) {
            receiptNprint[receipt.first] = receipt.second;
        }
        topicsIdSub = client.topicsIdSub;
    }

}

//copy assignment
Client &Client::operator=(const Client &client) {
    if (this == &client) return *this;
    else {
        //clear this
        fromServerVec.clear();
        connectionHandler = nullptr;
        for (auto &it: inventory) {
            it.second->clear();
            delete (it.second);
        }
        inventory.clear();
        bookLender.clear();
        askedBooks.clear();
        receiptNprint.clear();
        // copy the data from client to this
        this->host = client.host;
        this->name = client.name;
        this->password = client.password;
        this->login = client.login;
        connectionHandler = client.connectionHandler;
        //connectionHandler = new ConnectionHandler(client.connectionHandler->getHost(),client.connectionHandler->getPort());
        for (auto topic: client.inventory) {
            list<string> *l = new list<string>;
            for (auto book : *topic.second) {
                l->push_back(book);
            }
            inventory[topic.first] = l;
        }
        for (auto book: client.bookLender) {
            bookLender[book.first] = book.second;
        }
        for (size_t i = 0; i < client.askedBooks.size(); i++) {
            askedBooks.push_back(client.askedBooks[i]);
        }
        for (auto receipt: client.receiptNprint) {
            receiptNprint[receipt.first] = receipt.second;
        }
        topicsIdSub = client.topicsIdSub;
        return *this;
    }
}







//************************************additional functions runThreadKeyboard**************************************


//************************************additional functions runThreadSocket***************************************

