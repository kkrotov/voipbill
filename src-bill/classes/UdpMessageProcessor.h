#pragma once

#include <string>

#include "Repository.h"

using namespace std;

class UdpMessageProcessor {
public:

    UdpMessageProcessor(const string &message);

    string process();

private:
    string message;
    string prefix;
    string aNumber;
    string aNumberForAuth;
    string bNumber;
    string redirectionNumber;
    string trunkName;

    Repository repository;
    Server * server;
    Trunk * trunk;

    void init();
    void parseRequest();
    bool validateRequest();

    int processRouteTable(const int routeTableId);

    bool filterByNumber(const int numberId, string strNumber);

    string processOutcome(int outcomeId);

    string processAutoOutcome(Outcome * outcome);
    string processRouteCaseOutcome(Outcome * outcome);
    string processReleaseReasonOutcome(Outcome * outcome);
    string processAirpOutcome(Outcome * outcome);

    bool isLocalTrunk();
    bool needSwapCallingAndRedirectionNumber();
    bool canRouteForOperator(Trunk * oper);
    bool isOperatorRulesMatched(Trunk * oper, bool outgoing, string strNumber);
};

