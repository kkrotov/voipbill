#pragma once

#include "Billing.h"

class BillingCall {
protected:
    Billing *billing;
    Call *call;
    Cdr *cdr;
    PreparedData *data;

    Trunk *trunk;

public:
    BillingCall(Billing *billing);
    void calc(Call *call, Cdr *cdr, PreparedData *preparedData);

protected:
    void numberPreprocessing();
    void fillGeoPrefix();
    int getDest(GeoPrefix * geoPrefix);
    void calcByTrunk();
    void calcByNumber();
    void calcOrigByTrunk(ServiceTrunk *serviceTrunk);
    void calcTermByTrunk(ServiceTrunk *serviceTrunk);
    void calcOrigByNumber(ServiceNumber *serviceNumber);
    void calcTermByNumber(ServiceNumber *serviceNumber);

    char * getNumber();
    char * getRemoteNumber();
    char * getRoute();
    char * getRemoteRoute();

    bool filterByNumber(int numberId, char * str);
    bool filterByPrefixlist(int prefixlistId, char * str);

    int getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes);
    bool isUsage7800();
};

