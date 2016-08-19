#pragma once

#include <string>
#include <map>

#include "Repository.h"
//#include "RadiusAuthServer.h"

enum RejectReason {
    REASON_NO_BALANCE,
    REASON_DAILY_LIMIT,
};

using namespace std;

class RadiusAuthProcessor {
public:

    RadiusAuthProcessor(struct RadiusAuthRequest *request, struct RadiusAuthResponse *response,
                        pLogMessage &logRequest);

    void setTrace(stringstream *trace);

    void process() {
        static std::map<int, std::pair<RejectReason, time_t> > accountIdsBlockedBefore;
        time_t time_now = time(0);
        for (auto it = accountIdsBlockedBefore.begin(); it != accountIdsBlockedBefore.end(); it++) {

            std::pair<RejectReason, time_t> block = it->second;
            if (block.second <= time_now) {
                // время блокировки истекло, удаляем из списка
                accountIdsBlockedBefore.erase(it);
            }
        }
        process(&accountIdsBlockedBefore);
    }

    void process(std::map<int, std::pair<RejectReason, time_t> > *);

private:
    bool billingNotReady;
    stringstream *trace;
    struct RadiusAuthRequest *request;
    struct RadiusAuthResponse *response;
    pLogMessage logRequest;

    string aNumber;
    string bNumber;

    Repository repository;
    Server *server;
    Trunk *origTrunk;
    Client *account;

    void init();

    int processRouteTable(const int routeTableId);

    bool filterByNumber(const int numberId, string strNumber);

    bool processOutcome(int outcomeId, double *pBuyRate = 0, Pricelist **pFirstBuyPricelist = 0);

    bool processAutoOutcome(double *pBuyRate = 0, Pricelist **pFirstBuyPricelist = 0);

    void processRouteCaseOutcome(Outcome *outcome);

    void processReleaseReasonOutcome(Outcome *outcome);

    void processAirpOutcome(Outcome *outcome);

    void getAvailableOrigServiceTrunk(ServiceTrunk **origServiceTrunk, Pricelist **origPricelist,
                                      PricelistPrice **origPrice, ServiceTrunkSettings **origSettings);

    void getAvailableTermServiceTrunk(vector<ServiceTrunkOrder> &termServiceTrunks, Pricelist *origPricelist,
                                      PricelistPrice *origPrice, ServiceTrunkSettings *origSettings);

    bool processAutoRouteResponse(vector<ServiceTrunkOrder> &termOrders, double *pBuyRate = 0,
                                  Pricelist **pFirstBuyPricelist = 0, double origRub = 0);

    string analyzeCall(Call &call, std::map<int, std::pair<RejectReason, time_t> > *);

    bool isLowBalance(bool (Client::*lpCheckLimit)(double), RejectReason reason, Client *client, double spentBalanceSum,
                      Call &call, std::map<int, std::pair<RejectReason, time_t> > *);

    void fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double vat_rate);

    void processRedirectNumber();

    void processLineWithoutNumber();

    bool autoTrunkFilterSrcTrunk(Trunk *termTrunk);

    bool autoTrunkFilterSrcNumber(Trunk *termTrunk);

    bool autoTrunkFilterDstNumber(Trunk *termTrunk);

    bool matchTrunkGroup(const int trunkGroupId, const int trunkId);

    bool matchPrefixlist(const int prefixlistId, string strNumber);
};

