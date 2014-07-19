#pragma once

#include <string>

using namespace std;

class UdpMessageProcessor {
public:

    UdpMessageProcessor(const string &message);

    string process();

private:
    string message;
    string callingStationId;
    string calledStationId;

    void parseRequest();
    bool validateRequest();

};

