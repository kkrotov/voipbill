#pragma once

#include "../models/obj.h"
#include "../../src/classes/BDb.h"
#include <string>

using namespace std;

class UdpMessageProcessor {
public:

    UdpMessageProcessor(const string &message, BDb * db_calls);

    string process();

private:
    BDb * db_calls;
    string message;
    string aNumber;
    string bNumber;
    int trunk;
    CallObj call;


    void parseRequest();
    bool validateRequest();

    void prepareCall();
    void calculateCall();
    string analyzeCall();
};

