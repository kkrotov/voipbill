#pragma once

#include "Billing.h"

class BillingCall {
protected:
    Billing *billing;
    Call *call;
    Cdr *cdr;
    PreparedData *data;

    Trunk *trunk;

    stringstream *trace;

public:
    BillingCall(Billing *billing);
    void setTrace(stringstream *trace);
    void calc(Call *call, Cdr *cdr, PreparedData *preparedData);

protected:
    void numberPreprocessing();
    void processRedirectNumber();
    void fillGeoPrefix();
    int getDest(GeoPrefix * geoPrefix);
    void calcByTrunk();
    void calcByNumber();
    bool checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price);
    void calcOrigByNumber(ServiceNumber *serviceNumber);
    void calcTermByNumber(ServiceNumber *serviceNumber);

    void processLineWithoutNumber(Call *call, Cdr *cdr);

    long long int getNumber();
    long long int getRemoteNumber();
    char * getRoute();
    char * getRemoteRoute();

    bool filterByNumber(int numberId, long long int str);
    bool filterByPrefixlist(int prefixlistId, char * str);

    int getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes);
    bool isUsage7800();
};

