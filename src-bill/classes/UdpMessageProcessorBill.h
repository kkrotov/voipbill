#pragma once

#include "BDb.h"
#include <string>

using namespace std;

class UdpMessageProcessorBill {
public:

    UdpMessageProcessorBill(const string &message, BDb * db_calls);

    string process();

private:
    BDb * db_calls;
    string message;
    string aNumber;
    string bNumber;
    int trunkNumber;


    void parseRequest();
    bool validateRequest();

    void prepareCall();
    void calculateCall();
    string analyzeCall();
};

