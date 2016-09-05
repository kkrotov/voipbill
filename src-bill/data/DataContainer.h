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
#include "TrunkGroupData.h"
#include "TrunkGroupItemData.h"
#include "TrunkByNameData.h"
#include "TrunkByAliasData.h"
#include "TrunkNumberPreprocessingData.h"
#include "TrunkPriorityData.h"
#include "TrunkRuleData.h"
#include "TrunkTrunkRuleData.h"
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
#include "ServiceNumberByTechNumberData.h"
#include "ServiceNumberPackageData.h"
#include "ServiceTrunkData.h"
#include "ServiceTrunkPackageData.h"
#include "ServiceTrunkSettingsData.h"
#include "TariffData.h"
#include "TariffPackageData.h"
#include "TariffChangeLogData.h"
#include "GlobalCountersData.h"
#include "StatDestinationPrefixlistsData.h"
#include "StatPrefixlistData.h"
#include "ActiveCounterData.h"
#include "CurrencyRateData.h"

#include "nnp/NNPDestinationData.h"
#include "nnp/NNPNumberRangePrefixData.h"
#include "nnp/NNPPackageData.h"
#include "nnp/NNPPackagePriceData.h"
#include "nnp/NNPPrefixData.h"
#include "nnp/NNPRegionData.h"
#include "nnp/NNPNumberRangeData.h"
#include "nnp/NNPOperatorData.h"
#include "nnp/NNPPackageMinuteData.h"
#include "nnp/NNPPackagePricelistData.h"
#include "nnp/NNPPrefixDestinationData.h"
#include "nnp/NNPAccountTariffLightData.h"


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
    TrunkGroupData trunkGroup;
    TrunkGroupItemData trunkGroupItem;
    TrunkByNameData trunkByName;
    TrunkByAliasData trunkByAlias;
    TrunkNumberPreprocessingData trunkNumberPreprocessing;
    TrunkPriorityData trunkPriority;
    TrunkRuleData trunkRule;
    TrunkTrunkRuleData trunkTrunkRule;
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
    ServiceNumberByTechNumberData serviceNumberByTechNumber;
    ServiceNumberPackageData serviceNumberPackage;
    ServiceTrunkData serviceTrunk;
    ServiceTrunkPackageData serviceTrunkPackage;
    ServiceTrunkSettingsData serviceTrunkSettings;
    TariffData tariff;
    TariffPackageData tariffPackage;
    TariffChangeLogData tariffChangeLog;
    GlobalCountersData globalCounters;
    StatDestinationPrefixlistsData statDestinationPrefixlists;
    StatPrefixlistData statPrefixlist;
    ActiveCounterData activeCounter;
    CurrencyRateData currencyRate;

    NNPDestinationData nnpDestination;
    NNPNumberRangePrefixData nnpNumberRangePrefix;
    NNPPackageData nnpPackage;
    NNPPackagePriceData nnpPackagePrice;
    NNPPrefixData nnpPrefix;
    NNPRegionData nnpRegion;
    NNPNumberRangeData nnpNumberRange;
    NNPOperatorData nnpOperator;
    NNPPackageMinuteData nnpPackageMinute;
    NNPPackagePricelistData nnpPackagePricelist;
    NNPPrefixDestinationData nnpPrefixDestination;
    NNPAccountTariffLightData nnpAccountTariffLight;

    static DataContainer * instance();

    void loadAll(BDb * db);
};
