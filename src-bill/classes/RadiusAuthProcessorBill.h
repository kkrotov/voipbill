#pragma once

#include <string>
#include "../models/Call.h"
#include "../models/CallInfo.h"

using namespace std;

class RadiusAuthProcessorBill {
public:

    RadiusAuthProcessorBill(const string &aNumber, const string &bNumber, const string &trunkName);

    string process();

private:
    string aNumber;
    string bNumber;
    string trunkName;
    Call origCall;
    Call termCall;
    CallInfo origCallInfo;
    CallInfo termCallInfo;

    void prepareCall();
    void calculateCall();
    string analyzeCall();
};

