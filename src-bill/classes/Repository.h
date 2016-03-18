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

    TrunkGroup * getTrunkGroup(int trunk_group_id) {
        return trunkGroup->find(trunk_group_id, trace);
    }

    void getAllTrunkGroupTrunkIds(vector<int> &resultTrunkIds, int trunk_group_id) {
        trunkGroupItem->findTrunkIds(resultTrunkIds, trunk_group_id, trace);
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

    void getAllServiceTrunkSettings(vector<ServiceTrunkSettings *> &resultTrunkSettings, int trunk_id, int destinationType) {
        serviceTrunkSettings->findAll(resultTrunkSettings, trunk_id, destinationType, trace);
    }

    void getAllServiceNumberPackage(vector<ServicePackage *> &resultPackages, int service_number_id) {
        serviceNumberPackage->findAll(resultPackages, service_number_id, currentTime, trace);
    }

    void getAllServiceTrunkPackage(vector<ServicePackage *> &resultPackages, int service_trunk_id) {
        serviceTrunkPackage->findAll(resultPackages, service_trunk_id, currentTime, trace);
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


    RouteTable * getRouteTable(int id) {
        return routeTable->find(id, trace);
    }

    void getAllRouteTableRoutes(vector<RouteTableRoute *> &resultRoutes, int route_table_id) {
        routeTableRoute->findAll(resultRoutes, route_table_id, trace);
    }

    Outcome * getOutcome(int id) {
        return outcome->find(id, trace);
    }

    RouteCase * getRouteCase(int id) {
        return routeCase->find(id, trace);
    }

    ReleaseReason * getReleaseReason(int id) {
        return releaseReason->find(id, trace);
    }

    Airp * getAirp(int id) {
        return airp->find(id, trace);
    }

    Number * getNumber(int id) {
        return number->find(id, trace);
    }

    Prefixlist * getPrefixlist(int id) {
        return prefixlist->find(id, trace);
    }

    PrefixlistPrefix * getPrefixlistPrefix(int prefixlist_id, const char * prefix) {
        return prefixlistPrefix->find(prefixlist_id, prefix, trace);
    }

    void getAllTrunkRules(vector<TrunkRule *> &resultRules, int trunk_id, bool outgoing) {
        trunkRule->findAll(resultRules, trunk_id, outgoing, trace);
    }

    void getAllTrunkTrunkRulesTrunkGroupIds(vector<int> &resultTrunkGroupIds, int trunk_id) {
        trunkTrunkRule->findTrunkGroupIds(resultTrunkGroupIds, trunk_id, trace);
    }

    void getAllAutoRoutingTrunks(vector<Trunk *> &resultTrunks) {
        trunk->findAllAutorouting(resultTrunks, trace);
    }

    StatPrefixlist * getStatPrefixlist(int stat_prefixlist_id) {
        return statPrefixlist->find(stat_prefixlist_id, trace);
    }

    void getAllStatPrefixlistIdsByStatDestinationId(vector<int> &resultPrefixlistIds, int destination_id) {
        statDestinationPrefixlists->findAll(resultPrefixlistIds, destination_id, trace);
    }

    bool getCurrencyRate(const char* currency_id, double* o_currencyRate) const {
        if (!currency_id || !o_currencyRate) {
            throw Exception("Invalid arguments passed into getCurrencyRate()");
        }

        const CurrencyRate* rate = this->currencyRate->find(currency_id);
        if (rate) {
            * o_currencyRate = rate->rate;
            return true;
        } else {
            // "Безопасный" курс - 1:1
            * o_currencyRate = 1.0;
            return false;
        }
    }

    double priceToRoubles(double price, const Pricelist &pricelist) const {
        double currencyRate = 1.0;
        if (this->getCurrencyRate(pricelist.currency_id, & currencyRate)
            && currencyRate > 0.00000001) {
            return price * currencyRate;
        } else {
            return price;
        }
    }

    bool priceLessThan(double priceLeft, const Pricelist &pricelistLeft,
                       double priceRight, const Pricelist &pricelistRight) const {
        if (0 == strncmp(pricelistLeft.currency_id, pricelistRight.currency_id, 4)) {
            return priceLeft < priceRight;
        } else {
            double leftRoubles = this->priceToRoubles(priceLeft, pricelistLeft);
            double rightRoubles = this->priceToRoubles(priceRight, pricelistRight);
            return leftRoubles < rightRoubles;
        }
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

    bool trunkOrderLessThan(const ServiceTrunkOrder & left, const ServiceTrunkOrder & right) const {
        if (left.price && left.pricelist && right.price && right.pricelist) {
            return priceLessThan(left.price->price, *left.pricelist, right.price->price, *right.pricelist);
        }

        if (left.price && left.pricelist && (!right.price || !right.pricelist)) {
            // Известная цена всегда строго меньше любой неизвестной цены.
            return true;
        }

        if ((!left.price || !left.pricelist) && right.price && right.pricelist) {
            // Неизвестная цена никогда не меньше любой известной цены.
            return false;
        }

        // Если у обоих нет ценника, неважно, что вернуть - мы всё равно не сможем гарантировать стабильность сортировки
        return false;
    }

    struct trunk_settings_order_desc_price {
        bool operator() (const ServiceTrunkOrder & left, const ServiceTrunkOrder & right) {
            return this->repository.trunkOrderLessThan(right, left);
        }

        trunk_settings_order_desc_price(const Repository& repository) : repository(repository) {}

    private:
        const Repository& repository;
    };

    struct trunk_settings_order_asc_price {
        bool operator() (const ServiceTrunkOrder & left, const ServiceTrunkOrder & right) {
            return this->repository.trunkOrderLessThan(left, right);
        }

        trunk_settings_order_asc_price(const Repository& repository) : repository(repository) {}

    private:
        const Repository& repository;
    };

    void orderOrigTrunkSettingsOrderList(vector<ServiceTrunkOrder> &trunkSettingsOrderList) const {
        sort(trunkSettingsOrderList.begin(), trunkSettingsOrderList.end(), trunk_settings_order_asc_price(*this));
    }

    void orderTermTrunkSettingsOrderList(vector<ServiceTrunkOrder> &trunkSettingsOrderList) const {
        sort(trunkSettingsOrderList.begin(), trunkSettingsOrderList.end(), trunk_settings_order_asc_price(*this));
    }

    bool checkTrunkSettingsConditions(ServiceTrunkSettings * &trunkSettings, long long int srcNumber, long long int dstNumber, Pricelist * &pricelist, PricelistPrice * &price) {

        if (trunkSettings->src_number_id > 0 && !matchNumber(trunkSettings->src_number_id, srcNumber)) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK SETTINGS DECLINE|BY SRC NUMBER MATCHING, TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << "\n";
            }
            return false;
        }

        if (trunkSettings->dst_number_id > 0 && !matchNumber(trunkSettings->dst_number_id, dstNumber)) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK SETTINGS DECLINE|BY DST NUMBER MATCHING, TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << "\n";
            }
            return false;
        }

        pricelist = getPricelist(trunkSettings->pricelist_id);
        if (pricelist == nullptr) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK SETTINGS DECLINE|PRICELIST NOT FOUND BY ID: " << trunkSettings->pricelist_id << ", TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << "\n";
            }
            return false;
        }

        if (pricelist->local) {

            auto networkPrefix = getNetworkPrefix(pricelist->local_network_config_id, dstNumber);
            if (networkPrefix == nullptr) {
                if (trace != nullptr) {
                    *trace << "DEBUG|TRUNK SETTINGS DECLINE|NETWORK PREFIX NOT FOUND BY local_network_config_id: " << pricelist->local_network_config_id << ", PRICELIST_ID: " << pricelist->id << ", TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << " / " << "\n";
                }
                return false;
            }

            price = getPrice(trunkSettings->pricelist_id, networkPrefix->network_type_id);
            if (price == nullptr) {
                if (trace != nullptr) {
                    *trace << "DEBUG|TRUNK SETTINGS DECLINE|PRICE NOT FOUND: PRICELIST_ID: " << pricelist->id  << ", PREFIX: " << networkPrefix->network_type_id << ", TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << "\n";
                }
                return false;
            }


        } else {

            price = getPrice(trunkSettings->pricelist_id, dstNumber);
            if (price == nullptr) {
                if (trace != nullptr) {
                    *trace << "DEBUG|TRUNK SETTINGS DECLINE|PRICE NOT FOUND: PRICELIST_ID: " << pricelist->id  << ", PREFIX: " << dstNumber << ", TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << "\n";
                }
                return false;
            }

        }

        if (trace != nullptr) {
            *trace << "DEBUG|TRUNK SETTINGS ACCEPT|TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << "\n";
        }

        return true;
    }

    void getTrunkSettingsOrderList(vector<ServiceTrunkOrder> &resultTrunkSettingsTrunkOrderList, Trunk * trunk, long long int srcNumber, long long int dstNumber, int destinationType) {
        vector<ServiceTrunk *> serviceTrunks;
        getAllServiceTrunk(serviceTrunks, trunk->id);

        if (serviceTrunks.size() == 0) {
            if (trace != nullptr) {
                *trace << "DEBUG|SERVICE TRUNK DECLINE|CAUSE SERVICE TRUNK NOT FOUND BY TRUNK " << trunk->name << " (" << trunk->id << ")" << "\n";
            }
            return;
        }

        for (auto serviceTrunk : serviceTrunks) {
            vector<ServiceTrunkSettings *> trunkSettingsList;
            getAllServiceTrunkSettings(trunkSettingsList, serviceTrunk->id, destinationType);

            for (auto trunkSettings : trunkSettingsList) {
                Pricelist *pricelist;
                PricelistPrice *price;
                if (checkTrunkSettingsConditions(trunkSettings, srcNumber, dstNumber, pricelist, price)) {
                    auto account = getAccount(serviceTrunk->client_account_id);
                    if (account == nullptr) {
                        if (trace != nullptr) {
                            *trace << "DEBUG|TRUNK SETTINGS SKIP|ACCOUNT NOT FOUND BY ID " << serviceTrunk->client_account_id << "\n";
                        }
                        continue;
                    }
                    ServiceTrunkOrder order;
                    order.trunk = trunk;
                    order.serviceTrunk = serviceTrunk;
                    order.trunkSettings = trunkSettings;
                    order.pricelist = pricelist;
                    order.price = price;
                    resultTrunkSettingsTrunkOrderList.push_back(order);
                }
            }

        }

        if (resultTrunkSettingsTrunkOrderList.size() > 0) {
            if (trace != nullptr) {
                *trace << "FOUND|TRUNK SETTING ORDER LIST|" << "\n";
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|TRUNK SETTING ORDER LIST|" << "\n";
            }
        }
    }
};
