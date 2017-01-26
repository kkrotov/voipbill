#pragma once

#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"
#include "../data/DataCurrentCallsContainer.h"
#include "RadiusAuthRequestResponse.h"
#include "../models/Price.h"
#include "../lists/nnp/NNPCountryCodeList.h"

class Repository {
public:
    DataContainer *data;
    DataBillingContainer *billingData;
    DataCurrentCallsContainer *currentCalls;
    stringstream *trace;

private:
    time_t currentTime;

    shared_ptr<AirpList> airp;
    shared_ptr<ServerList> server;
    shared_ptr<InstanceSettingsList> instanceSettings;
    shared_ptr<HubList> hub;
    shared_ptr<NumberList> number;
    shared_ptr<OutcomeList> outcome;
    shared_ptr<PrefixlistList> prefixlist;
    shared_ptr<PrefixlistPrefixList> prefixlistPrefix;
    shared_ptr<ReleaseReasonList> releaseReason;
    shared_ptr<RouteCaseList> routeCase;
    shared_ptr<RouteTableList> routeTable;
    shared_ptr<RouteTableRouteList> routeTableRoute;
    shared_ptr<TrunkList> trunk;
    shared_ptr<TrunkGroupList> trunkGroup;
    shared_ptr<TrunkGroupItemList> trunkGroupItem;
    shared_ptr<TrunkByNameList> trunkByName;
    shared_ptr<TrunkByAliasList> trunkByAlias;
    shared_ptr<TrunkNumberPreprocessingList> trunkNumberPreprocessing;
    shared_ptr<TrunkPriorityList> trunkPriority;
    shared_ptr<TrunkRuleList> trunkRule;
    shared_ptr<TrunkTrunkRuleList> trunkTrunkRule;
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
    shared_ptr<ServiceNumberByTechNumberList> serviceNumberByTechNumber;
    shared_ptr<ServiceNumberPackageList> serviceNumberPackage;
    shared_ptr<ServiceTrunkList> serviceTrunk;
    shared_ptr<ServiceTrunkPackageList> serviceTrunkPackage;
    shared_ptr<ServiceTrunkSettingsList> serviceTrunkSettings;
    shared_ptr<TariffList> tariff;
    shared_ptr<TariffPackageList> tariffPackage;
    shared_ptr<TariffChangeLogList> tariffChangeLog;
    shared_ptr<StatDestinationPrefixlistsList> statDestinationPrefixlists;
    shared_ptr<StatPrefixlistList> statPrefixlist;
    shared_ptr<CurrencyRateList> currencyRate;

    shared_ptr<NNPRegionList> nnpRegion;
    shared_ptr<NNPDestinationList> nnpDestination;
    shared_ptr<NNPOperatorList> nnpOperator;
    shared_ptr<NNPPrefixList> nnpPrefix;
    shared_ptr<NNPCountryCodeList> nnpCountryCodeList;
    shared_ptr<NNPNumberRangeList> nnpNumberRange;
    shared_ptr<NNPNumberRangePrefixList> nnpNumberRangePrefix;
    shared_ptr<NNPPrefixDestinationList> nnpPrefixDestination;
    shared_ptr<NNPPackageList> nnpPackage;
    shared_ptr<NNPPackageMinuteList> nnpPackageMinute;
    shared_ptr<NNPPackagePriceList> nnpPackagePrice;
    shared_ptr<NNPPackagePricelistList> nnpPackagePricelist;
    shared_ptr<NNPAccountTariffLightList> nnpAccountTariffLight;

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

    Trunk *getServiceTrunk(int trunk_settings_id, ServiceTrunkSettings &trunkSettings);

    bool prepare(time_t currentTime = 0);

    InstanceSettings *getInstanceSettings(int instance_id) {
        return instanceSettings->find(instance_id);
    }

    Server *getServer(int instance_id) {
        return server->find(instance_id);
    }

    Hub *getHub(int instance_id) {
        return hub->find(instance_id);
    }

    void getServersByHubId(vector<Server> &servers, int hub_id) {
        if (hub_id > 0 && server != nullptr)
            server->getServersByHubId(servers, hub_id);
    }

    Client *getAccount(int account_id) {
        return client->find(account_id, trace);
    }

    Trunk *getTrunk(int trunk_id) {
        return trunk->find(trunk_id, trace);
    }

    Trunk *findAnyOurTrunk(int server_id) {
        return trunk->findAnyOurTrunk(server_id, trace);
    }

    Trunk *getTrunkByName(const char *trunk_name);

    TrunkGroup *getTrunkGroup(int trunk_group_id) {
        return trunkGroup->find(trunk_group_id, trace);
    }

    void getAllTrunkGroupTrunkIds(vector<int> &resultTrunkIds, int trunk_group_id) {
        trunkGroupItem->findTrunkIds(resultTrunkIds, trunk_group_id, trace);
    }

    ServiceNumber *getServiceNumber(PhoneNumber numberPrefix) {
        return serviceNumber->find(numberPrefix, currentTime, trace);
    }

    ServiceNumber *getServiceNumber(const char *numberPrefix) {
        return getServiceNumber(atoll(numberPrefix));
    }

    ServiceNumber *getServiceNumberByTechnicalNumber(long long int technicalNumber) {
        return serviceNumberByTechNumber->find(technicalNumber, currentTime, trace);
    }

    ServiceTrunk *getServiceTrunk(int trunk_id) {
        return serviceTrunk->find(trunk_id, currentTime, trace);
    }

    void getAllServiceTrunk(vector<ServiceTrunk *> &resultTrunks, int trunk_id) {
        serviceTrunk->findAll(resultTrunks, trunk_id, currentTime, trace);
    }

    void getAllServiceTrunkSettings(vector<ServiceTrunkSettings *> &resultTrunkSettings, int trunk_id,
                                    int destinationType) {
        serviceTrunkSettings->findAll(resultTrunkSettings, trunk_id, destinationType, trace);
    }

    void getAllServiceNumberPackage(vector<ServicePackage *> &resultPackages, int service_number_id) {
        serviceNumberPackage->findAll(resultPackages, service_number_id, currentTime, trace);
    }

    void getAllServiceTrunkPackage(vector<ServicePackage *> &resultPackages, int service_trunk_id) {
        serviceTrunkPackage->findAll(resultPackages, service_trunk_id, currentTime, trace);
    }

    Pricelist *getPricelist(int pricelist_id) {
        return pricelist->find(pricelist_id, trace);
    }

    PricelistPrice *getPrice(int pricelist_id, long long int numberPrefix) {
        return pricelistPrice->find(pricelist_id, numberPrefix, currentTime, trace);
    }

    NetworkPrefix *getNetworkPrefix(int network_config_id, long long int numberPrefix) {
        return networkPrefix->find(network_config_id, numberPrefix, currentTime, trace);
    }

    TariffChangeLog *getTariffLog(int usage_id) {
        return tariffChangeLog->find(usage_id, currentTime, trace);
    }

    Tariff *getTariff(int tariff_id) {
        return tariff->find(tariff_id, trace);
    }

    TariffPackage *getTariffPackage(int tariff_id) {
        return tariffPackage->find(tariff_id, trace);
    }

    MobPrefix *getMobPrefix(long long int prefix) {
        return mobPrefix->find(prefix, trace);
    }

    GeoPrefix *getGeoPrefix(long long int prefix) {
        return geoPrefix->find(prefix, trace);
    }

    Geo *getGeo(int geo_id) {
        return geo->find(geo_id, trace);
    }

    TrunkNumberPreprocessing *getTrunkNumberPreprocessing(int trunk_id, int order) {
        return trunkNumberPreprocessing->find(trunk_id, order, trace);
    }


    RouteTable *getRouteTable(int id) {
        return routeTable->find(id, trace);
    }

    void getAllRouteTableRoutes(vector<RouteTableRoute *> &resultRoutes, int route_table_id) {
        routeTableRoute->findAll(resultRoutes, route_table_id, trace);
    }

    Outcome *getOutcome(int id) {
        return outcome->find(id, trace);
    }

    RouteCase *getRouteCase(int id) {
        return routeCase->find(id, trace);
    }

    ReleaseReason *getReleaseReason(int id) {
        return releaseReason->find(id, trace);
    }

    Airp *getAirp(int id) {
        return airp->find(id, trace);
    }

    Number *getNumber(int id) {
        return number->find(id, trace);
    }

    Prefixlist *getPrefixlist(int id) {
        return prefixlist->find(id, trace);
    }

    PrefixlistPrefix *getPrefixlistPrefix(int prefixlist_id, const char *prefix) {
        return prefixlistPrefix->find(prefixlist_id, prefix, trace);
    }

    void getAllTrunkRules(vector<TrunkRule *> &resultRules, int trunk_id, bool outgoing) {
        trunkRule->findAll(resultRules, trunk_id, outgoing, trace);
    }

    void getAllTrunkTrunkRulesTrunkGroupIds(vector<int> &resultTrunkGroupIds, int trunk_id) {
        trunkTrunkRule->findTrunkGroupIds(resultTrunkGroupIds, trunk_id, trace);
    }

    void getAllTrunkGroupRules(vector<TrunkTrunkRule> &resultTrunkGroupRules, int trunk_id) {
        trunkTrunkRule->findTrunkGroupRules(resultTrunkGroupRules, trunk_id, trace);
    }

    void getAllAutoRoutingTrunks(vector<Trunk *> &resultTrunks, int server_id) {
        trunk->findAllAutorouting(resultTrunks, server_id, trace);
    }

    void getAllRoadToRegion(vector<Trunk *> &resultTrunks, int server_id, int road_to_region,
                            stringstream *trace = nullptr) {
        trunk->findAllRoadToRegion(resultTrunks, server_id, road_to_region, trace);
    }

    StatPrefixlist *getStatPrefixlist(int stat_prefixlist_id) {
        return statPrefixlist->find(stat_prefixlist_id, trace);
    }

    void getAllStatPrefixlistIdsByStatDestinationId(vector<int> &resultPrefixlistIds, int destination_id) {
        statDestinationPrefixlists->findAll(resultPrefixlistIds, destination_id, trace);
    }

    bool getCurrencyRate(const char *currency_id, double *o_currencyRate) const;

    double priceToRoubles(double price, const Pricelist &pricelist) const;

    double priceToRoubles(double price, const char *currency_id) const;

    bool priceLessThan(double priceLeft, const Pricelist &pricelistLeft,
                       double priceRight, const Pricelist &pricelistRight) const;


    bool matchNumber(int number_id, long long int numberPrefix);

    bool matchPrefixlist(int prefixlist_id, char *prefix);

    double getVatRate(Client *client);

    bool trunkOrderLessThan(const ServiceTrunkOrder &left, const ServiceTrunkOrder &right) const;

    struct trunk_settings_order_desc_price {
        bool operator()(const ServiceTrunkOrder &left, const ServiceTrunkOrder &right) {
            return this->repository.trunkOrderLessThan(right, left);
        }

        trunk_settings_order_desc_price(const Repository &repository) : repository(repository) {}

    private:
        const Repository &repository;
    };

    struct trunk_settings_order_asc_price {
        bool operator()(const ServiceTrunkOrder &left, const ServiceTrunkOrder &right) {
            return this->repository.trunkOrderLessThan(left, right);
        }

        trunk_settings_order_asc_price(const Repository &repository) : repository(repository) {}

    private:
        const Repository &repository;
    };

    struct trunk_settings_order_asc_priority {
        bool operator()(const ServiceTrunkOrder &left, const ServiceTrunkOrder &right) {
            return left.priority > right.priority;
        }

        trunk_settings_order_asc_priority(const Repository &repository) : repository(repository) {}

    private:
        const Repository &repository;
    };

    void orderOrigTrunkSettingsOrderList(vector<ServiceTrunkOrder> &trunkSettingsOrderList) const {
        sort(trunkSettingsOrderList.begin(), trunkSettingsOrderList.end(), trunk_settings_order_asc_price(*this));
    }

    void orderTermTrunkSettingsOrderList(vector<ServiceTrunkOrder> &trunkSettingsOrderList, bool fUseMinimalki,
                                         time_t connect_time) const;

    bool checkTrunkSettingsOldPricelistConditions(ServiceTrunkSettings *&trunkSettings, long long int srcNumber,
                                                  long long int dstNumber, Pricelist *&pricelist,
                                                  PricelistPrice *&price);

    void getActiveNNPAccountTariffLight(vector<NNPAccountTariffLight> &resultNNPAccountTariffLight, int client_id,
                                        time_t connect_time, int service_number_id) {

        nnpAccountTariffLight->findAllActiveByClientID(resultNNPAccountTariffLight, client_id, connect_time,
                                                       service_number_id, trace);
    }

    void getServiceNumberByClientID(vector<ServiceNumber> &resultServiceNumber, int client_id) {
        serviceNumber->findAllByClientID(resultServiceNumber, client_id, trace);
    }

    void getServiceTrunkByClientID(vector<ServiceTrunk> &resultServiceTrunk, int client_id) {
        serviceTrunk->findAllByClientID(resultServiceTrunk, client_id, trace);
    }

    void getNNPPackageMinuteByTariff(vector<NNPPackageMinute> &resultNNPPackageMinute, int nnp_tariff_id,
                                     double coefficient) {
        nnpPackageMinute->findAllByTariffID(resultNNPPackageMinute, nnp_tariff_id, coefficient, trace);
    }

    NNPDestination *getNNPDestination(int id, stringstream *trace = nullptr) {
        return nnpDestination->find(id, trace);
    }

    NNPNumberRange *getNNPNumberRange(long long int num, stringstream *trace = nullptr) {
        return nnpNumberRange->getNNPNumberRange(num, trace);
    }

    int getNNPCountryCode(int country_prefix) {

        return nnpCountryCodeList->get_code_by_prefix(country_prefix);
    }

    NNPPackageMinute *getNNPPackageMinute(int idNNPPackageMinute, stringstream *trace = nullptr) {
        return nnpPackageMinute->find(idNNPPackageMinute, trace);
    }

    NNPPackage *getNNPPackage(int idNNPPackage, stringstream *trace = nullptr) {
        return nnpPackage->find(idNNPPackage, trace);
    }

    bool getNNPPrefixsByNumberRange(vector<int> &nnpPrefixIds,
                                    int nnpNumberRangeId, stringstream *trace = nullptr) {
        return nnpNumberRangePrefix->getNNPPrefixsByNumberRange(nnpPrefixIds,
                                                                nnpNumberRangeId, trace);
    }

    bool getNNPDestinationsByPrefix(set<int> &nnpDestinationIds,
                                    vector<int> &nnpNumberRangePrefixList,
                                    stringstream *trace = nullptr) {
        return nnpPrefixDestination->getNNPDestinationsByPrefix(nnpDestinationIds, nnpNumberRangePrefixList, trace);
    }

    bool getNNPDestinationByNum(set<int> &nnpDestinationIds, long long int num, stringstream *trace = nullptr);

    bool getPrefixByNNPDestination(vector<PhoneNumber> &prefixList, int destinationId);

    bool getNNPDestinationByNumberRange(set<int> &nnpDestinationIds, NNPNumberRange *nnpNumberRange,
                                        stringstream *trace = nullptr);

    void findNNPPackagePriceIds(set<pair<double, NNPPackagePrice *>> &resultNNPPackagePriceIds, int tariff_id,
                                set<int> &nnpDestinationIds,
                                stringstream *trace = nullptr) {
        return nnpPackagePrice->findPackagePriceIds(resultNNPPackagePriceIds, tariff_id, nnpDestinationIds, trace);
    }

    void
    findNNPPackagePricelistIds(set<pair<double, NNPPackagePricelist *>> &resultNNPPackagePricelistIds, int tariff_id,
                               long long int num, stringstream *trace = nullptr);


    PhoneNumber getNNPBestGeoRoute(PhoneNumber NumAdef, vector<PhoneNumber> &vNumA, PhoneNumber NumB,
                                   stringstream *trace = nullptr);

    void getNNPBestPriceRoute(set<pair<double, PhoneNumber>> &vResNum, vector<PhoneNumber> &vNumA, PhoneNumber NumB,
                              stringstream *trace = nullptr);

    pair<int, RadiusAuthRequest> getNNPRegionTrunkByNum(PhoneNumber numA, PhoneNumber numB);

    void getTrunkPriority(int trunk_id, vector<TrunkPriority> &trunkPriorityList);

    void getTrunkSettingsOrderList(vector<ServiceTrunkOrder> &resultTrunkSettingsTrunkOrderList, Trunk *trunk,
                                   long long int srcNumber, long long int dstNumber, int destinationType);

    bool checkNNPTrunkSettingsConditions(ServiceTrunkSettings *&trunkSettings, long long int srcNumber,
                                         long long int dstNumber);

    void setCurrencyRate(Price &price) const;

    bool isRegionOnHub(int region) {
        return server->isRegionOnHub(region);
    }

};
