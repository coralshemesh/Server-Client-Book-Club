//
// Created by alonakor@wincs.cs.bgu.ac.il on 07/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_STOMPFRAME_H
#define BOOST_ECHO_CLIENT_STOMPFRAME_H

#include <string>
#include <map>

class Stompframe {

private:
    std::string action;
    std::map<std::string,std::string> headers;
    std::string body;

public:
    Stompframe(std::string, std::map<std::string,std::string> , std::string);

    Stompframe(const Stompframe &);// copy constructor

   // void encodeFrame(ConnectionHandler&);

    std::string toString();

    bool equalAction(std::string);

    std::string getValueByKey(std::string);

    bool bodyContains(std::string);

    std::string getBody();

};


#endif //BOOST_ECHO_CLIENT_STOMPFRAME_H
