#pragma once

#include <string>

#include "ConfigVersionData.h"

using namespace std;

class UdpMessageProcessor {
public:

    UdpMessageProcessor(const string &message);

    string process();

private:
    string message;
    string prefix;
    string aNumber;
    string bNumber;
    string redirectionNumber;
    int trunk;

    shared_ptr<ConfigVersionData> data;

    void parseRequest();
    bool validateRequest();

    int processRouteTable(const int routeTableId);

    bool filterByNumber(const int numberId, string strNumber);

    string processOutcome(int outcomeId);

    string processAutoOutcome(pOutcome outcome);
    string processRouteCaseOutcome(pOutcome outcome);
    string processReleaseReasonOutcome(pOutcome outcome);
    string processAirpOutcome(pOutcome outcome);
    
    bool isConnectedOperator();
    bool needSwapCallingAndRedirectionNumber();
};

