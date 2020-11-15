//
// Created by alonakor@wincs.cs.bgu.ac.il on 11/01/2020.
//

#include <boost/algorithm/string.hpp>
#include "KeyBoardReader.h"


using namespace std;
using boost::asio::ip::tcp;


KeyBoardReader::KeyBoardReader(Client &client, mutex &inventoryMutex, mutex &bookLenderMutex) : exitFlag(false),
                                                                                                input(),
                                                                                                connectionHandler(
                                                                                                        client.getCon()),
                                                                                                subId(0),
                                                                                                client(&client),
                                                                                                inventoryMutex(
                                                                                                        &inventoryMutex),
                                                                                                bookLenderMutex(
                                                                                                        &bookLenderMutex) {}

void KeyBoardReader::runThreadKeyboard() {
    if (!client->loggedIn()) {
        login();
        client->setLoggedin(true);
    }
    while (!exitFlag & client->loggedIn()) {
        //read IO from keyboard and push it into a vector
        string inputString;
        getline(cin, inputString);
        istringstream s(inputString);
        do {
            string word;
            s >> word;
            this->input.push_back(word);
        } while (s);
        if (client->loggedIn()) {
            if (input[0] == "login")login();
            else if (input[0] == ("join")) { join(input[1]); }
            else if (input[0] == ("exit")) { exit(input[1]); }
            else if (input[0] == ("add") || (input[0] == ("borrow"))) {
                string bookName;
                for (size_t i = 2; i < input.size(); i++) { //bookName can be more than one word
                    bookName += input[i] + " ";
                }
                bookName = bookName.substr(0, bookName.size() - 2);
                if (input[0] == ("add"))
                    addBook(input[1], bookName);
                else borrowBook(input[1], bookName);
            }   //in this thread the method only sends a SEND frame and doesnt check anything
            else if (input[0] == ("return")) {
                string bookName;
                //bookName can be more than one word so...
                for (size_t i = 2; i < input.size(); i++) { bookName += input[i] + " "; }
                bookName = bookName.substr(0, bookName.size() - 2); // remove the last " "
                returnBook(input[1], bookName);
            } else if (input[0] == ("status")) { bookStatus(input[1]); }
            else if (input[0] == ("logout")) { logout(); }
            else cout << "invalid action" << endl;
            input.clear();
        } else exitFlag = true;
    }
}

void KeyBoardReader::login() {

    // create headers map
    map<string, string> headers;
    headers["accept-version"] = "1.2";
    headers["host"] = client->getHost();
    headers["login"] = client->getName();
    headers["passcode"] = client->getPassword();
    Stompframe stompframe("CONNECT", headers, "");
    connectionHandler->encodeFrame(stompframe);
}


void KeyBoardReader::join(string genre) {
    subId = subId + 1;
    client->addToTopicsIdSub(genre, subId);
    map<string, string> headers;
    headers["destination"] = genre;
    headers["id"] = to_string(subId);
    client->incrementReceiptId();
    headers["receipt"] = to_string(client->getReceiptId());
    client->setAndAdd(client->getReceiptId(), "joined club " + genre);
    Stompframe stompframe("SUBSCRIBE", headers, "");
    connectionHandler->encodeFrame(stompframe);
    cout << "sent SUBSCRIBE frame" << endl;
}


void KeyBoardReader::exit(string genre) {
    if (client->findTopicsIdSub(genre)) {
        map<string, string> headers;
        headers["id"] = to_string(client->returnIdSub(genre));
        client->incrementReceiptId();
        headers["receipt"] = to_string(client->getReceiptId());
        client->setAndAdd(client->getReceiptId(), "Exited club " + genre);
        Stompframe stompframe("UNSUBSCRIBE", headers, "");
        connectionHandler->encodeFrame(stompframe);
        cout << "sent UNSUBSCRIBE frame" << endl;
    } else cout << "I'm not subscribed to this topic !!!" << endl;
}

void KeyBoardReader::addBook(string genre, string book) {
    inventoryMutex->lock();
    client->addToInventory(genre, book);
    inventoryMutex->unlock();
    cout << client->getName() + " added " + book << endl;

    map<string, string> headers;
    headers["destination"] = genre;
    Stompframe stompframe("SEND", headers, client->getName() + " has added the book " + book);
    connectionHandler->encodeFrame(stompframe);
}

void KeyBoardReader::borrowBook(string genre, string bookName) {
    client->addToAskedBooks(bookName);
    map<string, string> headers;
    headers["destination"] = genre;
    Stompframe stompframe("SEND", headers, client->getName() + " wish to borrow " + bookName);
    connectionHandler->encodeFrame(stompframe);
}

void KeyBoardReader::returnBook(string genre, string bookName) {
    inventoryMutex->lock();
    if (client->inventoryContains(genre, bookName)) {
        client->removeFromInventory(genre, bookName);// remove the book from the user's inventory
        inventoryMutex->unlock();
        map<string, string> headers;
        headers["destination"] = genre;
        bookLenderMutex->lock();
        Stompframe stompframe("SEND", headers,
                              "Returning " + bookName + " to " +
                              client->lenderName(bookName));
        client->removeFromBookLender(
                bookName); // return the book to his owner and delete this data from the clients map
        bookLenderMutex->unlock();
        connectionHandler->encodeFrame(stompframe);
    } else {
        cout << "I don't have this book!!!" << endl;
        inventoryMutex->unlock();
    }
}

void KeyBoardReader::bookStatus(string genre) {
    //send a SEND frame - book status
    map<string, string> headers;
    headers["destination"] = genre;
    Stompframe stompframe("SEND", headers, "book status");
    connectionHandler->encodeFrame(stompframe);
}

void KeyBoardReader::logout() {
    exitFlag = true;
    client->clearTopicsIdSub(); // remove all subscription id for this user
    map<string, string> headers;
    client->incrementReceiptId();
    headers["receipt"] = to_string(client->getReceiptId());
    client->setAndAdd(client->getReceiptId(), "logged out");
    Stompframe stompframe("DISCONNECT", headers, "");
    connectionHandler->encodeFrame(stompframe);
}

KeyBoardReader::~KeyBoardReader() {

    connectionHandler = nullptr;
    client = nullptr;
    inventoryMutex = nullptr;
    bookLenderMutex = nullptr;

}

KeyBoardReader::KeyBoardReader(const KeyBoardReader &keyBoardReader)
        : exitFlag(), input(), connectionHandler(), subId(), client(), inventoryMutex(), bookLenderMutex() {

    if (this != &keyBoardReader) {
        exitFlag = keyBoardReader.exitFlag;
        for (string s:keyBoardReader.input)
            input.push_back(s);
        connectionHandler = new ConnectionHandler(keyBoardReader.connectionHandler->getHost(),keyBoardReader.connectionHandler->getPort());
        subId = keyBoardReader.subId;
        client = keyBoardReader.client;
        inventoryMutex = keyBoardReader.inventoryMutex;
        bookLenderMutex = keyBoardReader.bookLenderMutex;
    }
}

KeyBoardReader &KeyBoardReader::operator=(const KeyBoardReader &keyBoardReader) {
    if (this == &keyBoardReader) return *this;
    else {
        //clear this
        connectionHandler = nullptr;
        client = nullptr;
        inventoryMutex = nullptr;
        bookLenderMutex = nullptr;
        //copy from keyBoardReader to this
        exitFlag = keyBoardReader.exitFlag;
        for (string s:keyBoardReader.input)
            input.push_back(s);
        subId = keyBoardReader.subId;
        connectionHandler = new ConnectionHandler(keyBoardReader.connectionHandler->getHost(),keyBoardReader.connectionHandler->getPort());
        client = keyBoardReader.client;
        inventoryMutex = keyBoardReader.inventoryMutex;
        bookLenderMutex = keyBoardReader.bookLenderMutex;
        return *this;
    }
}













