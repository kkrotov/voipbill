#pragma once

#include <string>

#include "Repository.h"
#include "RadiusAuthServer.h"

using namespace std;

struct ServiceTrunkOrder {
    Trunk * trunk;
    ServiceTrunk * serviceTrunk;
    Pricelist * pricelist;
    PricelistPrice * price;
};

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
    void getAvailableOrigServiceTrunk(ServiceTrunk * origServiceTrunk, Pricelist * origPricelist, PricelistPrice * origPrice);
    void getAvailableTermServiceTrunk(vector<ServiceTrunkOrder> &termServiceTrunks);
    void processAutoRouteResponse(RadiusAuthResponse &response, vector<ServiceTrunkOrder> &termOrders);
    string analyzeCall(Call &call);
    void fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double vat_rate);

    bool needSwapCallingAndRedirectionNumber();

    bool autoTrunkFilterSrcNumber(Trunk * termTrunk);
    bool autoTrunkFilterDstNumber(Trunk * termTrunk);
    bool matchPrefixlist(const int prefixlistId, string strNumber);
    bool checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price);
};

