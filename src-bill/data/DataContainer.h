#pragma once

#include "../../src/classes/BDb.h"

#include "ServerData.h"
#include "InstanceSettingsData.h"
#include "AirpData.h"
#include "NumberData.h"
#include "OutcomeData.h"
#include "PrefixlistData.h"
#include "PrefixlistPrefixData.h"
#include "ReleaseReasonData.h"
#include "RouteCaseData.h"
#include "RouteTableData.h"
#include "RouteTableRouteData.h"
#include "TrunkData.h"
#include "TrunkNumberPreprocessingData.h"
#include "TrunkPriorityData.h"
#include "TrunkRuleData.h"
#include "ClientData.h"
#include "OperatorData.h"
#include "PricelistData.h"
#include "PricelistPriceData.h"
#include "NetworkPrefixData.h"
#include "GeoPrefixData.h"
#include "UsageData.h"
#include "ServiceNumberData.h"
#include "ServiceTrunkData.h"
#include "ServiceTrunkSettingsData.h"
#include "TariffData.h"
#include "TariffChangeLogData.h"

#include "CurrentCdrData.h"

class DataContainer {
public:
    ServerData server;
    InstanceSettingsData instanceSettings;
    AirpData airp;
    NumberData number;
    OutcomeData outcome;
    PrefixlistData prefixlist;
    PrefixlistPrefixData prefixlistPrefix;
    ReleaseReasonData releaseReason;
    RouteCaseData routeCase;
    RouteTableData routeTable;
    RouteTableRouteData routeTableRoute;
    TrunkData trunk;
    TrunkNumberPreprocessingData trunkNumberPreprocessing;
    TrunkPriorityData trunkPriority;
    TrunkRuleData trunkRule;
    ClientData client;
    OperatorData voipOperator;
    PricelistData pricelist;
    PricelistPriceData pricelistPrice;
    NetworkPrefixData networkPrefix;
    GeoPrefixData geoPrefix;
    UsageData usage;
    ServiceNumberData serviceNumber;
    ServiceTrunkData serviceTrunk;
    ServiceTrunkSettingsData serviceTrunkSettings;
    TariffData tariff;
    TariffChangeLogData tariffChangeLog;

    CurrentCdrData currentCdrData;

    static DataContainer * instance() {
        static DataContainer inst;
        return &inst;
    }

    void loadAll(BDb * db) {
        server.load(db);
        instanceSettings.load(db);
        airp.load(db);
        number.load(db);
        outcome.load(db);
        prefixlist.load(db);
        prefixlistPrefix.load(db);
        releaseReason.load(db);
        routeCase.load(db);
        routeTable.load(db);
        routeTableRoute.load(db);
        trunk.load(db);
        trunkNumberPreprocessing.load(db);
        trunkPriority.load(db);
        trunkRule.load(db);
        client.load(db);
        voipOperator.load(db);
        pricelist.load(db);
        pricelistPrice.load(db);
        networkPrefix.load(db);
        geoPrefix.load(db);
        usage.load(db);
        serviceNumber.load(db);
        serviceTrunk.load(db);
        serviceTrunkSettings.load(db);
        tariff.load(db);
        tariffChangeLog.load(db);

        currentCdrData.load(db);
    }
};