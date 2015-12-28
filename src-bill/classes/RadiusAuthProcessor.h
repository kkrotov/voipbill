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

    RadiusAuthProcessor(RadiusAuthRequest * request, RadiusAuthResponse * response, pLogMessage &logRequest);
    void setTrace(stringstream *trace);
    void process();

private:
    stringstream *trace;
    RadiusAuthRequest * request;
    RadiusAuthResponse * response;
    pLogMessage logRequest;

    string aNumber;
    string bNumber;

    Repository repository;
    Server * server;
    Trunk * origTrunk;
    Client * account;

    void init();

    int processRouteTable(const int routeTableId);

    bool filterByNumber(const int numberId, string strNumber);

    void processOutcome(int outcomeId);

    void processAutoOutcome();
    void processRouteCaseOutcome(Outcome * outcome);
    void processReleaseReasonOutcome(Outcome * outcome);
    void processAirpOutcome(Outcome * outcome);
    void getAvailableOrigServiceTrunk(ServiceTrunk * origServiceTrunk, Pricelist * origPricelist, PricelistPrice * origPrice);
    void getAvailableTermServiceTrunk(vector<ServiceTrunkOrder> &termServiceTrunks);
    void processAutoRouteResponse(vector<ServiceTrunkOrder> &termOrders);
    string analyzeCall(Call &call);
    void fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double vat_rate);

    void processRedirectNumber();
    void processLineWithoutNumber();

    bool autoTrunkFilterSrcTrunk(Trunk * termTrunk);
    bool autoTrunkFilterSrcNumber(Trunk * termTrunk);
    bool autoTrunkFilterDstNumber(Trunk * termTrunk);
    bool matchTrunkGroup(const int trunkGroupId, const int trunkId);
    bool matchPrefixlist(const int prefixlistId, string strNumber);
    bool checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price);
};

