#pragma once

#include "../models/obj.h"
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
    CallObj call;


    void parseRequest();
    bool validateRequest();

    void prepareCall();
    void calculateCall();
    string analyzeCall();
};

