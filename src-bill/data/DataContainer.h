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
#include "ServiceTrunkData.h"
#include "ServiceTrunkSettingsData.h"
#include "TariffData.h"
#include "TariffChangeLogData.h"
#include "GlobalCountersData.h"
#include "ActiveCounterData.h"

struct PreparedData {
    Server *server;
    InstanceSettings *instanceSettings;
    shared_ptr<AirpList> airp;
    shared_ptr<NumberList> number;
    shared_ptr<OutcomeList> outcome;
    shared_ptr<PrefixlistList> prefixlist;
    shared_ptr<PrefixlistPrefixList> prefixlistPrefix;
    shared_ptr<ReleaseReasonList> releaseReason;
    shared_ptr<RouteCaseList> routeCase;
    shared_ptr<RouteTableList> routeTable;
    shared_ptr<RouteTableRouteList> routeTableRoute;
    shared_ptr<TrunkList> trunk;
    shared_ptr<TrunkByNameList> trunkByName;
    shared_ptr<TrunkByAliasList> trunkByAlias;
    shared_ptr<TrunkNumberPreprocessingList> trunkNumberPreprocessing;
    shared_ptr<TrunkPriorityList> trunkPriority;
    shared_ptr<TrunkRuleList> trunkRule;
    shared_ptr<ClientList> client;
    shared_ptr<OrganizationList> organization;
    shared_ptr<OperatorList> voipOperator;
    shared_ptr<PricelistList> pricelist;
    shared_ptr<PricelistPriceList> pricelistPrice;
    shared_ptr<NetworkPrefixList> networkPrefix;
    shared_ptr<GeoList> geo;
    shared_ptr<MobPrefixList> mobPrefix;
    shared_ptr<GeoPrefixList> geoPrefix;
    shared_ptr<ServiceNumberList> serviceNumber;
    shared_ptr<ServiceTrunkList> serviceTrunk;
    shared_ptr<ServiceTrunkSettingsList> serviceTrunkSettings;
    shared_ptr<TariffList> tariff;
    shared_ptr<TariffChangeLogList> tariffChangeLog;
    shared_ptr<ActiveCounter> activeCounter;

    double getVatRate(Client * client)
    {
        if (client != nullptr) {
            auto org = organization->find(client->organization_id, time(nullptr));
            if (org != nullptr) {
                return org->vat_rate;
            }
        }
        return 0;
    }
};

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
    ServiceTrunkData serviceTrunk;
    ServiceTrunkSettingsData serviceTrunkSettings;
    TariffData tariff;
    TariffChangeLogData tariffChangeLog;
    GlobalCountersData globalCounters;
    ActiveCounterData activeCounter;

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
        trunkByName.load(db);
        trunkByAlias.load(db);
        trunkNumberPreprocessing.load(db);
        trunkPriority.load(db);
        trunkRule.load(db);
        client.load(db);
        organization.load(db);
        voipOperator.load(db);
        pricelist.load(db);
        pricelistPrice.load(db);
        networkPrefix.load(db);
        geo.load(db);
        mobPrefix.load(db);
        geoPrefix.load(db);
        serviceNumber.load(db);
        serviceTrunk.load(db);
        serviceTrunkSettings.load(db);
        tariff.load(db);
        tariffChangeLog.load(db);

        activeCounter.load(serviceNumber.get(), serviceTrunk.get());
    }

    bool prepareData(PreparedData &data, time_t time) {

        {
            shared_ptr<ServerList> list = server.get();
            if (list != nullptr) {
                auto item = list->find(app().conf.instance_id);
                if (item != nullptr) {
                    data.server = server.get()->find(app().conf.instance_id);
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        {
            shared_ptr<InstanceSettingsList> list = instanceSettings.get();
            if (list != nullptr) {
                auto item = list->find(app().conf.instance_id);
                if (item != nullptr) {
                    data.instanceSettings = item;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        if ((data.airp = airp.get()) == nullptr) {
            return false;
        }

        if ((data.number = number.get()) == nullptr) {
            return false;
        }

        if ((data.outcome = outcome.get()) == nullptr) {
            return false;
        }

        if ((data.prefixlist = prefixlist.get()) == nullptr) {
            return false;
        }

        if ((data.prefixlistPrefix = prefixlistPrefix.get()) == nullptr) {
            return false;
        }

        if ((data.releaseReason = releaseReason.get()) == nullptr) {
            return false;
        }

        if ((data.routeCase = routeCase.get()) == nullptr) {
            return false;
        }

        if ((data.routeTable = routeTable.get()) == nullptr) {
            return false;
        }

        if ((data.routeTableRoute = routeTableRoute.get()) == nullptr) {
            return false;
        }

        if ((data.trunk = trunk.get()) == nullptr) {
            return false;
        }

        if ((data.trunkByName = trunkByName.get()) == nullptr) {
            return false;
        }

        if ((data.trunkByAlias = trunkByAlias.get()) == nullptr) {
            return false;
        }

        if ((data.trunkNumberPreprocessing = trunkNumberPreprocessing.get()) == nullptr) {
            return false;
        }

        if ((data.trunkPriority = trunkPriority.get()) == nullptr) {
            return false;
        }

        if ((data.trunkRule = trunkRule.get()) == nullptr) {
            return false;
        }

        if ((data.client = client.get()) == nullptr) {
            return false;
        }

        if ((data.organization = organization.get()) == nullptr) {
            return false;
        }

        if ((data.voipOperator = voipOperator.get()) == nullptr) {
            return false;
        }

        if ((data.pricelist = pricelist.get()) == nullptr) {
            return false;
        }

        if ((data.pricelistPrice = pricelistPrice.get()) == nullptr) {
            return false;
        }

        if ((data.networkPrefix = networkPrefix.get()) == nullptr) {
            return false;
        }

        if ((data.geo = geo.get()) == nullptr) {
            return false;
        }

        if ((data.mobPrefix = mobPrefix.get()) == nullptr) {
            return false;
        }

        if ((data.geoPrefix = geoPrefix.get()) == nullptr) {
            return false;
        }

        if ((data.serviceNumber = serviceNumber.get()) == nullptr) {
            return false;
        }

        if ((data.serviceTrunk = serviceTrunk.get()) == nullptr) {
            return false;
        }

        if ((data.serviceTrunkSettings = serviceTrunkSettings.get()) == nullptr) {
            return false;
        }

        if ((data.tariff = tariff.get()) == nullptr) {
            return false;
        }

        if ((data.tariffChangeLog = tariffChangeLog.get()) == nullptr) {
            return false;
        }

        if ((data.activeCounter = activeCounter.get()) == nullptr) {
            return false;
        }

        return true;
    }
};