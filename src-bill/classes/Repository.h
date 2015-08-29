#pragma once

#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"
#include "../data/DataCurrentCallsContainer.h"

class Repository {
public:
    DataContainer *data;
    DataBillingContainer *billingData;
    DataCurrentCallsContainer *currentCalls;
    stringstream *trace;

private:
    time_t currentTime;
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
    shared_ptr<ServicePackageList> servicePackage;
    shared_ptr<ServiceTrunkList> serviceTrunk;
    shared_ptr<ServiceTrunkSettingsList> serviceTrunkSettings;
    shared_ptr<TariffList> tariff;
    shared_ptr<TariffPackageList> tariffPackage;
    shared_ptr<TariffChangeLogList> tariffChangeLog;
public:
    shared_ptr<ActiveCounter> activeCounter;

public:
    Repository() {
        data = DataContainer::instance();
        billingData = DataBillingContainer::instance();
        currentCalls = DataCurrentCallsContainer::instance();
        trace = nullptr;
        currentTime = time(nullptr);
    }

    bool prepare(time_t currentTime = 0);

    InstanceSettings * getInstanceSettings() {
        return instanceSettings;
    }

    Server * getServer() {
        return server;
    }

    Client * getAccount(int account_id) {
        return client->find(account_id, trace);
    }

    Trunk * getTrunk(int trunk_id) {
        return trunk->find(trunk_id, trace);
    }

    Trunk * getTrunkByName(const char *trunk_name) {
        Trunk * trunk = trunkByName->find(trunk_name, trace);
        if (trunk != nullptr) {
            return trunk;
        }

        trunk = trunkByAlias->find(trunk_name, trace);
        if (trunk != nullptr) {
            return trunk;
        }

        return nullptr;
    }

    ServiceNumber * getServiceNumber(long long int numberPrefix) {
        return serviceNumber->find(numberPrefix, currentTime, trace);
    }

    ServiceNumber * getServiceNumber(const char * numberPrefix) {
        return getServiceNumber(atoll(numberPrefix));
    }

    ServiceTrunk * getServiceTrunk(int trunk_id) {
        return serviceTrunk->find(trunk_id, currentTime, trace);
    }

    void getAllServiceTrunk(vector<ServiceTrunk *> &resultTrunks, int trunk_id) {
        serviceTrunk->findAll(resultTrunks, trunk_id, currentTime, trace);
    }

    void getAllServiceTrunkSettings(vector<ServiceTrunkSettings *> &resultTrunkSettings, int trunk_id, int type) {
        serviceTrunkSettings->findAll(resultTrunkSettings, trunk_id, type, trace);
    }

    void getAllServicePackage(vector<ServicePackage *> &resultPackages, int service_number_id) {
        servicePackage->findAll(resultPackages, service_number_id, currentTime, trace);
    }

    Pricelist * getPricelist(int pricelist_id) {
        return pricelist->find(pricelist_id, trace);
    }

    PricelistPrice * getPrice(int pricelist_id, long long int numberPrefix) {
        return pricelistPrice->find(pricelist_id, numberPrefix, currentTime, trace);
    }

    NetworkPrefix * getNetworkPrefix(int network_config_id, long long int numberPrefix) {
        return networkPrefix->find(network_config_id, numberPrefix, currentTime, trace);
    }

    TariffChangeLog * getTariffLog(int usage_id) {
        return tariffChangeLog->find(usage_id, currentTime, trace);
    }

    Tariff * getTariff(int tariff_id) {
        return tariff->find(tariff_id, trace);
    }

    TariffPackage * getTariffPackage(int tariff_id) {
        return tariffPackage->find(tariff_id, trace);
    }

    Number * getNumber(int number_id) {
        return number->find(number_id);
    }

    Prefixlist * getPrefixlist(int prefixlist_id) {
        return prefixlist->find(prefixlist_id);
    }

    PrefixlistPrefix * getPrefixlistPrefix(int prefixlist_id, char * prefix) {
        return prefixlistPrefix->find(prefixlist_id, prefix);
    }

    MobPrefix * getMobPrefix(long long int prefix) {
        return mobPrefix->find(prefix, trace);
    }

    GeoPrefix * getGeoPrefix(long long int prefix) {
        return geoPrefix->find(prefix, trace);
    }

    Geo * getGeo(int geo_id) {
        return geo->find(geo_id, trace);
    }

    TrunkNumberPreprocessing * getTrunkNumberPreprocessing(int trunk_id, int order) {
        return trunkNumberPreprocessing->find(trunk_id, order, trace);
    }

    bool matchNumber(int number_id, long long int numberPrefix) {
        char tmpNumber[20];
        sprintf(tmpNumber, "%lld", numberPrefix);

        auto number = getNumber(number_id);
        if (number == nullptr) {
            return false;
        }

        auto prefixlistIds = number->getPrefixlistIds();
        for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {
            if (matchPrefixlist(*it, tmpNumber)) {
                return true;
            }
        }

        return false;

    }

    bool matchPrefixlist(int prefixlist_id, char * prefix) {
        auto prefixlist = getPricelist(prefixlist_id);
        if (prefixlist == nullptr) {
            return false;
        }

        auto prefixlistPrefix = getPrefixlistPrefix(prefixlist->id, prefix);
        return prefixlistPrefix != nullptr;
    }

    double getVatRate(Client * client) {
        if (client != nullptr && !client->price_include_vat) {
            auto org = organization->find(client->organization_id, time(nullptr));
            if (org != nullptr) {
                return org->vat_rate;
            }
        }
        return 0;
    }


};