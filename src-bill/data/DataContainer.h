#pragma once

#include "../classes/BDb.h"

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
#include "TrunkByNameData.h"
#include "TrunkByAliasData.h"
#include "TrunkNumberPreprocessingData.h"
#include "TrunkPriorityData.h"
#include "TrunkRuleData.h"
#include "ClientData.h"
#include "OrganizationData.h"
#include "OperatorData.h"
#include "PricelistData.h"
#include "PricelistPriceData.h"
#include "NetworkPrefixData.h"
#include "GeoData.h"
#include "MobPrefixData.h"
#include "GeoPrefixData.h"
#include "ServiceNumberData.h"
#include "ServicePackageData.h"
#include "ServiceTrunkData.h"
#include "ServiceTrunkSettingsData.h"
#include "TariffData.h"
#include "TariffPackageData.h"
#include "TariffChangeLogData.h"
#include "GlobalCountersData.h"
#include "StatDestinationPrefixlistsData.h"
#include "StatPrefixlistData.h"
#include "ActiveCounterData.h"

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
    TrunkByNameData trunkByName;
    TrunkByAliasData trunkByAlias;
    TrunkNumberPreprocessingData trunkNumberPreprocessing;
    TrunkPriorityData trunkPriority;
    TrunkRuleData trunkRule;
    ClientData client;
    OrganizationData organization;
    OperatorData voipOperator;
    PricelistData pricelist;
    PricelistPriceData pricelistPrice;
    NetworkPrefixData networkPrefix;
    GeoData geo;
    MobPrefixData mobPrefix;
    GeoPrefixData geoPrefix;
    ServiceNumberData serviceNumber;
    ServicePackageData servicePackage;
    ServiceTrunkData serviceTrunk;
    ServiceTrunkSettingsData serviceTrunkSettings;
    TariffData tariff;
    TariffPackageData tariffPackage;
    TariffChangeLogData tariffChangeLog;
    GlobalCountersData globalCounters;
    StatDestinationPrefixlistsData statDestinationPrefixlists;
    StatPrefixlistData statPrefixlist;
    ActiveCounterData activeCounter;

    static DataContainer * instance();

    void loadAll(BDb * db);
};