//
// Created by alonakor@wincs.cs.bgu.ac.il on 11/01/2020.
//

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include "SocketReader.h"

using namespace std;

SocketReader::SocketReader(Client &client,mutex &inventoryMutex,mutex &bookLenderMutex) :connectionHandler(client.getCon()), client(&client),logoutFlag(false),inventoryMutex(&inventoryMutex),bookLenderMutex(&bookLenderMutex) {}

void SocketReader::runThreadSocket() {
    while (!logoutFlag) {
        vector<string> fromServerVec;
        string fromServer;
        connectionHandler->getLine(fromServer); //get a bytes array from server and copy it into fromServer
        //split the string we created from the data received from server
        boost::split(fromServerVec, fromServer, boost::is_any_of("\n"));
        Stompframe frame = toStompframe(fromServerVec);
        string body = frame.getBody();
        vector<string> bodyVec;
        boost::split(bodyVec, body, boost::is_any_of(" "));

        cout << frame.toString() << endl;

        if (frame.equalAction("CONNECTED")) { cout << "login successfully" << endl; }
        else if (frame.equalAction("RECEIPT")) {
            string receipt = frame.getValueByKey("receipt-id");
            string firstWord = client->receiptHandle(receipt);
            if ("logged" == firstWord) {
                logoutFlag = true;
                connectionHandler->close();
            }
        } else if (frame.equalAction("MESSAGE")) {
            int end = findLastWord(&bodyVec);
            if (frame.bodyContains("borrow")) {
                borrowMessage(getBookName(4,end, &bodyVec));
            } else if (frame.bodyContains("Returning")) {
                returnMessage(bodyVec[end], getBookName(1,end-2,&bodyVec), frame.getValueByKey("destination"));
            } else if (frame.bodyContains("has") & !frame.bodyContains("added")) {
                hasMessage(getBookName(2,end, &bodyVec), bodyVec[0],frame.getValueByKey("destination"));
            } else if (frame.bodyContains("Taking")) {
                takingMessage(bodyVec[end], getBookName(1,end-2,&bodyVec), frame.getValueByKey("destination"));
            } else if (frame.bodyContains("status")) {
                statusMessage(frame.getValueByKey("destination"));
            }
        } else if (frame.equalAction("ERROR")) {
            logoutFlag = true;
            client->setLoggedin(false);
            connectionHandler->close();
        }
    }
    connectionHandler->close();
}


void SocketReader::borrowMessage(string book) {
    map<string, string> headers;
    bool found = false;
    inventoryMutex->lock();
    for (auto it = client->getInventory()->begin(); it != client->getInventory()->end() && !found; ++it) {
        for (string s :*it->second) {
            if (s == book) {
                headers["destination"] = it->first;
                Stompframe stompframe("SEND", headers, client->getName() + " has " + s);
                connectionHandler->encodeFrame(stompframe);
                found = true;
            }
        }
    }
    inventoryMutex->unlock();
}

void SocketReader::hasMessage(string book, string owner,string topic) {
    std::vector<std::string> *askedBooks = client->getAskedBooks();
    if (find(askedBooks->begin(), askedBooks->end(), book) != askedBooks->end()) { //if I asked for this book
        map<string, string> headers;
        headers["destination"] = topic; //book topic
        askedBooks->erase(find(askedBooks->begin(), askedBooks->end(), book)); //remove book from askedBook:
        bookLenderMutex->lock();
        client->addToBookLender(book, owner);
        bookLenderMutex->unlock();
        inventoryMutex->lock();
        client->addToInventory(topic, book);
        inventoryMutex->unlock();
        Stompframe stompframe("SEND", headers, "Taking " + book + " from " + owner);
        connectionHandler->encodeFrame(stompframe);
    }
}

void SocketReader::takingMessage(string lender, string book, string topic) {
    if (lender == client->getName()) { //if someone took the book from me
        inventoryMutex->lock();
        client->removeFromInventory(topic,book); // remove the book from the lender inventory
        inventoryMutex->unlock();
    }

}

void SocketReader::statusMessage(string topic) {
    map<string, string> headers;
    headers["destination"] = topic;
    //send a SEND frame - list of my books
   // inventoryMutex->lock();
    string body = client->getName() + ":" + client->createStringInventory();
    //send a SEND frame with a list of the books I have
    body.pop_back(); // remove the last comma in the string
    //inventoryMutex->unlock();
    Stompframe stompframe1("SEND", headers, body);
    connectionHandler->encodeFrame(stompframe1);
}

void SocketReader::returnMessage(string lender, string book, string topic) {
    inventoryMutex->lock();
    if (lender == client->getName()) { //if its my book
        client->addToInventory(topic,book);
    }
    inventoryMutex->unlock();
}

Stompframe SocketReader::toStompframe(vector<string> dataFromServer) {
    string msgAction = dataFromServer[0];
    map<string, string> msgHeaders;
    size_t i = 1;
    while (dataFromServer[i] != "") {
        // split each header
        vector<string> arr;
        boost::split(arr, dataFromServer[i], boost::is_any_of(":"));
        msgHeaders[arr[0]] = arr[1]; // insert key:value to the headers map
        i++;
    }

    i++;
    string msgBody = dataFromServer[i];

    return Stompframe(msgAction, msgHeaders, msgBody);
}

std::string SocketReader::getBookName(size_t from, size_t to, vector<string> *bodyVec) { // finds the book name from the body string
    string bookName;
    for (size_t i = from; i <= to; i++) // bookName start at index 4
            bookName += bodyVec->at(i) + " ";
    bookName.pop_back();
    return bookName;
}

int SocketReader::findLastWord(std::vector<std::string> * bodyVec) {
    size_t i;
    bool found=false;
    for(i = bodyVec->size()-1; (i >= 0)& !found; i--){
        if(bodyVec->at(i) != "") found = true;
    }
    return i+1;
}

SocketReader::~SocketReader() {
    connectionHandler = nullptr;
    client = nullptr;
    inventoryMutex = nullptr;
    bookLenderMutex = nullptr;
}


//copy assignment
SocketReader &SocketReader::operator=(const SocketReader &socketReader) {
    if (this == &socketReader)
        return  *this;
    else {
        //clear this
        connectionHandler = nullptr;
        client = nullptr;
        inventoryMutex = nullptr;
        bookLenderMutex = nullptr;
        //copy data from socketReader to this
        connectionHandler = new ConnectionHandler(socketReader.connectionHandler->getHost(), socketReader.connectionHandler->getPort()) ;
        logoutFlag=socketReader.logoutFlag;
        client = new Client(socketReader.connectionHandler->getHost(), socketReader.client->getName(), socketReader.client->getPassword(), *this->connectionHandler);
        inventoryMutex = socketReader.inventoryMutex;
        bookLenderMutex = socketReader.bookLenderMutex;
        return *this;
    }
}

//copy constructor
SocketReader::SocketReader(const SocketReader& socketReader):connectionHandler(), client(),logoutFlag(),inventoryMutex(),bookLenderMutex()  {
    if ( this != &socketReader){
        connectionHandler = new ConnectionHandler(socketReader.connectionHandler->getHost(), socketReader.connectionHandler->getPort()) ;
        logoutFlag=socketReader.logoutFlag;
        client = new Client(socketReader.connectionHandler->getHost(), socketReader.client->getName(), socketReader.client->getPassword(), *this->connectionHandler);
        inventoryMutex = socketReader.inventoryMutex;
        bookLenderMutex = socketReader.bookLenderMutex;
    }

}





