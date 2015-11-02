#pragma once

#include <string>

#include "Repository.h"
#include "RadiusAuthServer.h"

using namespace std;

class RadiusAuthProcessor {
public:

    void process(RadiusAuthRequest &request, RadiusAuthResponse &response);

private:
    string prefix;
    string aNumber;
    string bNumber;
    string redirectionNumber;
    string trunkName;

    Repository repository;
    Server * server;
    Trunk * origTrunk;

    void init();

    int processRouteTable(const int routeTableId);

    bool filterByNumber(const int numberId, string strNumber);

    void processOutcome(RadiusAuthResponse &response, int outcomeId);

    void processAutoOutcome(RadiusAuthResponse &response);
    void processRouteCaseOutcome(RadiusAuthResponse &response, Outcome * outcome);
    void processReleaseReasonOutcome(RadiusAuthResponse &response, Outcome * outcome);
    void processAirpOutcome(RadiusAuthResponse &response, Outcome * outcome);

    bool needSwapCallingAndRedirectionNumber();

    bool autoTrunkFilterSrcNumber(Trunk * termTrunk);
    bool autoTrunkFilterDstNumber(Trunk * termTrunk);
    bool matchPrefixlist(const int prefixlistId, string strNumber);
    bool checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price);
};

