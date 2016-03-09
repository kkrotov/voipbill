#pragma once

#include "Repository.h"

class BillingCall {
protected:
    Repository *repository;
    Call *call;
    CallInfo *callInfo;
    Cdr *cdr;

    stringstream *trace;

public:
    BillingCall(Repository *repository);
    void setTrace(stringstream *trace);
    void clearTrace();
    void calc(Call *call, CallInfo * callInfo, Cdr *cdr);

protected:
    void calcByTrunk();
    void calcByNumber();
    void calcOrigByNumber();
    void calcTermByNumber();
    void numberPreprocessing();
    void processRedirectNumber();
    void processGeo();
    void processDestinations();
    int getDest(int geo_id);
    void processLineWithoutNumber(Call *call, Cdr *cdr);
    long long int getNumber();
    long long int getRemoteNumber();
    char * getRoute();
    char * getRemoteRoute();
    int getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes);
    bool isUsage7800();

    void setupTrunk();
    void setupEffectiveOrigTrunkSettings();
    void setupEffectiveTermTrunkSettings();
    void setupServiceTrunk();
    void setupServiceNumber();
    void setupServiceTrunkForNumber();
    void setupAccount();
    void setupPricelist(int pricelist_id = 0);
    void setupPrice(long long int numberPrefix = 0);
    void setupBilledTime();
    void setupCost();
    void setupLogTariff();
    void setupMainTariff();
    void setupTariff();
    void setupPackagePricelist();
    void setupFreemin();
    void setupPackagePrepaid();

    bool matchTariffPackageDestination(TariffPackage * tariff);

};

