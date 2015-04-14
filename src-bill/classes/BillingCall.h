#pragma once

#include "Billing.h"

class BillingCall {
protected:
    Billing *billing;
    Call *call;
    Cdr *cdr;

    ServerList *serverList;
    InstanceSettingsList *instanceSettingsList;
    TrunkList *trunkList;
    TrunkNumberPreprocessingList *trunkNumberPreprocessingList;
    GeoPrefixList *geoPrefixList;
    ServiceNumberList *serviceNumberList;
    ServiceTrunkList *serviceTrunkList;
    ServiceTrunkSettingsList *serviceTrunkSettingsList;
    TariffList *tariffList;
    TariffChangeLogList *tariffChangeLogList;
    PricelistList *pricelistList;
    PricelistPriceList *pricelistPriceList;
    NetworkPrefixList *networkPrefixList;
    OperatorList *operatorList;
    NumberList *numberList;
    PrefixlistList *prefixlistList;
    PrefixlistPrefixList *prefixlistPrefixList;

    Server *server;
    InstanceSettings *instanceSettings;
    Trunk *trunk;


public:
    BillingCall(Billing *billing);
    bool init(Call *call, Cdr *cdr);
    void calc();

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

