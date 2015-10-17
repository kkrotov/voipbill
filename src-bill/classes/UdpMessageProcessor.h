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
    string bNumber;
    string redirectionNumber;
    string trunkName;

    Repository repository;
    Server * server;
    Trunk * origTrunk;

    void init();
    void parseRequest();
    bool validateRequest();

    int processRouteTable(const int routeTableId);

    bool filterByNumber(const int numberId, string strNumber);

    string processOutcome(int outcomeId);

    string processAutoOutcome();
    string processRouteCaseOutcome(Outcome * outcome);
    string processReleaseReasonOutcome(Outcome * outcome);
    string processAirpOutcome(Outcome * outcome);

    bool isLocalTrunk();
    bool needSwapCallingAndRedirectionNumber();

    bool autoTrunkFilterSrcNumber(Trunk * termTrunk);
    bool autoTrunkFilterDstNumber(Trunk * termTrunk);
    bool matchPrefixlist(const int prefixlistId, string strNumber);
    bool checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price);
};

