//
// Created by alonakor@wincs.cs.bgu.ac.il on 07/01/2020.
//
#include "Stompframe.h"
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;


Stompframe::Stompframe(string action, map<string, string> headers, string body) : action(action), headers(headers),
                                                                                  body(body) {}

Stompframe::Stompframe(const Stompframe &stompframe) : action(), headers(), body() {
    if (this != &stompframe) {
        for (auto it : stompframe.headers) {
            headers[it.first] = it.second;
        }
        action = stompframe.action;
        body = stompframe.body;
    }
}

string Stompframe::toString() {
    string result;
    result = result + action + "\n";
    for (auto s : headers) {
        result += s.first + ":" + s.second + "\n";
    }
    result += "\n" + body;
    /* + "\n" + '\0'*/
    return result;

}

/*void Stompframe::encodeFrame(ConnectionHandler &connectionHandler) {
    connectionHandler.sendFrameAscii(this->toString(), '\n');
}*/

bool Stompframe::equalAction(string action) { return this->action == action; }

string Stompframe::getValueByKey(string key) {
    return headers.find(key)->second;
}

bool Stompframe::bodyContains(string keyword) {
    return boost::algorithm::contains(body, keyword);
}

string Stompframe::getBody() {
    return body;
}











