#include "Repository.h"

bool Repository::prepare(time_t currentTime) {

    this->currentTime = currentTime == 0 ? time(nullptr) : currentTime;

    if ((hub = data->hub.get()) == nullptr) {
        return false;
    }
    {
        if (app().conf.hub_id > 0) {
            auto item = hub->find(app().conf.hub_id);
            if (item == nullptr) false;
        }
    }

    if ((server = data->server.get()) == nullptr) {
        return false;
    }
    {
        auto item = server->find(app().conf.instance_id);
        if (item == nullptr) false;
    }

    if ((instanceSettings = data->instanceSettings.get()) == nullptr) {
        return false;
    }
    {
        auto item = instanceSettings->find(app().conf.instance_id);
        if (item == nullptr) false;
    }


    if ((airp = data->airp.get()) == nullptr) {
        return false;
    }

    if ((number = data->number.get()) == nullptr) {
        return false;
    }

    if ((outcome = data->outcome.get()) == nullptr) {
        return false;
    }

    if ((prefixlist = data->prefixlist.get()) == nullptr) {
        return false;
    }

    if ((prefixlistPrefix = data->prefixlistPrefix.get()) == nullptr) {
        return false;
    }

    if ((releaseReason = data->releaseReason.get()) == nullptr) {
        return false;
    }

    if ((routeCase = data->routeCase.get()) == nullptr) {
        return false;
    }

    if ((routeTable = data->routeTable.get()) == nullptr) {
        return false;
    }

    if ((routeTableRoute = data->routeTableRoute.get()) == nullptr) {
        return false;
    }

    if ((trunk = data->trunk.get()) == nullptr) {
        return false;
    }

    if ((trunkGroup = data->trunkGroup.get()) == nullptr) {
        return false;
    }

    if ((trunkGroupItem = data->trunkGroupItem.get()) == nullptr) {
        return false;
    }

    if ((trunkByName = data->trunkByName.get()) == nullptr) {
        return false;
    }

    if ((trunkByAlias = data->trunkByAlias.get()) == nullptr) {
        return false;
    }

    if ((trunkNumberPreprocessing = data->trunkNumberPreprocessing.get()) == nullptr) {
        return false;
    }

    if ((trunkPriority = data->trunkPriority.get()) == nullptr) {
        return false;
    }

    if ((trunkRule = data->trunkRule.get()) == nullptr) {
        return false;
    }

    if ((trunkTrunkRule = data->trunkTrunkRule.get()) == nullptr) {
        return false;
    }

    if ((client = data->client.get()) == nullptr) {
        return false;
    }

    if ((organization = data->organization.get()) == nullptr) {
        return false;
    }

    if ((voipOperator = data->voipOperator.get()) == nullptr) {
        return false;
    }

    if ((pricelist = data->pricelist.get()) == nullptr) {
        return false;
    }

    if ((pricelistPrice = data->pricelistPrice.get()) == nullptr) {
        return false;
    }

    if ((networkPrefix = data->networkPrefix.get()) == nullptr) {
        return false;
    }

    if ((geo = data->geo.get()) == nullptr) {
        return false;
    }

    if ((mobPrefix = data->mobPrefix.get()) == nullptr) {
        return false;
    }

    if ((geoPrefix = data->geoPrefix.get()) == nullptr) {
        return false;
    }

    if ((serviceNumber = data->serviceNumber.get()) == nullptr) {
        return false;
    }

    if ((serviceNumberByTechNumber = data->serviceNumberByTechNumber.get()) == nullptr) {
        return false;
    }

    if ((serviceNumberPackage = data->serviceNumberPackage.get()) == nullptr) {
        return false;
    }

    if ((serviceTrunk = data->serviceTrunk.get()) == nullptr) {
        return false;
    }

    if ((serviceTrunkPackage = data->serviceTrunkPackage.get()) == nullptr) {
        return false;
    }

    if ((serviceTrunkSettings = data->serviceTrunkSettings.get()) == nullptr) {
        return false;
    }

    if ((tariff = data->tariff.get()) == nullptr) {
        return false;
    }

    if ((tariffPackage = data->tariffPackage.get()) == nullptr) {
        return false;
    }

    if ((tariffChangeLog = data->tariffChangeLog.get()) == nullptr) {
        return false;
    }

    if ((statDestinationPrefixlists = data->statDestinationPrefixlists.get()) == nullptr) {
        return false;
    }

    if ((statPrefixlist = data->statPrefixlist.get()) == nullptr) {
        return false;
    }

    if ((activeCounter = data->activeCounter.get()) == nullptr) {
        return false;
    }

    if ((currencyRate = data->currencyRate.get()) == nullptr) {
        return false;
    }


    if ((nnpDestination = data->nnpDestination.get()) == nullptr) {
        return false;
    }

    if ((nnpNumberRangePrefix = data->nnpNumberRangePrefix.get()) == nullptr) {
        return false;
    }

    if ((nnpPackage = data->nnpPackage.get()) == nullptr) {
        return false;
    }

    if ((nnpPackagePrice = data->nnpPackagePrice.get()) == nullptr) {
        return false;
    }

    if ((nnpPrefix = data->nnpPrefix.get()) == nullptr) {
        return false;
    }

    if ((nnpRegion = data->nnpRegion.get()) == nullptr) {
        return false;
    }

    if ((nnpNumberRange = data->nnpNumberRange.get()) == nullptr) {
        return false;
    }

    if ((nnpOperator = data->nnpOperator.get()) == nullptr) {
        return false;
    }

    if ((nnpPackageMinute = data->nnpPackageMinute.get()) == nullptr) {
        return false;
    }

    if ((nnpPackagePricelist = data->nnpPackagePricelist.get()) == nullptr) {
        return false;
    }

    if ((nnpPrefixDestination = data->nnpPrefixDestination.get()) == nullptr) {
        return false;
    }

    if ((nnpAccountTariffLight = data->nnpAccountTariffLight.get()) == nullptr) {
        return false;
    }

    return true;
}

void Repository::orderTermTrunkSettingsOrderList(vector<ServiceTrunkOrder> &trunkSettingsOrderList, bool fUseMinimalki,
                                                 time_t connect_time) const {

        vector<ServiceTrunkOrder> trunkSettingsOrderFreeList;
        vector<ServiceTrunkOrder> trunkSettingsOrderPayList;

        for (auto order : trunkSettingsOrderList) {
            order.statsTrunkSettings = billingData->statsTrunkSettingsGetCurrent(connect_time, order.account,
                                                                                 order.trunkSettings);

            bool isAcceptedTrunk = false;

            for (auto accepted_order : trunkSettingsOrderList)
                if (accepted_order.trunkSettings->id == order.trunkSettings->id) {
                    isAcceptedTrunk = true;
                }

            if (!isAcceptedTrunk) continue;

            if (order.trunkSettings->minimum_minutes > 0 && fUseMinimalki) {
                if (order.statsTrunkSettings->used_seconds < order.trunkSettings->minimum_minutes * 60) {
                    trunkSettingsOrderFreeList.push_back(order);
                    if (trace != nullptr) {
                        *trace << "DEBUG|TRUNK SETTINGS ORDER LIST|TRUNK_SETTINGS_ID: " << order.trunkSettings->id
                        << " / used_seconds  = " << order.statsTrunkSettings->used_seconds
                        << " / minimum_minutes * 60 = " << order.trunkSettings->minimum_minutes * 60 << "\n";
                    }
                    continue;
                }
            }

            if (order.trunkSettings->minimum_cost > 0 && fUseMinimalki) {
                if (order.statsTrunkSettings->used_credit <= order.trunkSettings->minimum_cost) {
                    trunkSettingsOrderFreeList.push_back(order);
                    if (trace != nullptr) {
                        *trace << "DEBUG|TRUNK SETTINGS ORDER LIST|TRUNK_SETTINGS_ID: " << order.trunkSettings->id
                        << " / used_credit = " << order.statsTrunkSettings->used_credit
                        << " / minimum_cost = " << order.trunkSettings->minimum_cost << "\n";
                    }
                    continue;
                }
            }
            trunkSettingsOrderPayList.push_back(order);
        }

        sort(trunkSettingsOrderFreeList.begin(), trunkSettingsOrderFreeList.end(),
             trunk_settings_order_asc_price(*this));
        sort(trunkSettingsOrderPayList.begin(), trunkSettingsOrderPayList.end(), trunk_settings_order_asc_price(*this));

        trunkSettingsOrderList.clear();

        for (auto order : trunkSettingsOrderFreeList) {
            trunkSettingsOrderList.push_back(order);
        }

        for (auto order : trunkSettingsOrderPayList) {
            trunkSettingsOrderList.push_back(order);
        }

}

void Repository::findNNPPackagePricelistIds(set<pair<double, int>> &resultNNPPackagePricelistIds, int tariff_id,
                                            long long int num, stringstream *trace) {

    set<NNPPackagePricelist *> nnpPackagePricelistPtr;


    nnpPackagePricelist->findPackagePricelistIds(nnpPackagePricelistPtr, tariff_id, num, trace);

    for (auto item : nnpPackagePricelistPtr) {
        NNPPackagePricelist *package = &*item;
        PricelistPrice *pricelistPrice = getPrice(package->pricelist_id, num);
        if (pricelistPrice != nullptr) {
            double cost = pricelistPrice->price;

            resultNNPPackagePricelistIds.insert(pair<double, int>(cost, package->id));

            if (trace != nullptr) {
                *trace << "FOUND|NNP PACKAGE PRICELIST|BY NNP_TARIFF_ID '" << tariff_id << "'" << "\n";
                *trace << "||";
                package->dump(*trace);
                *trace << "\n";
            }
        }

    }

}

Trunk *Repository::getServiceTrunk(int trunk_settings_id, ServiceTrunkSettings &trunkSettings) {
    if (serviceTrunkSettings == nullptr)
        return nullptr;

    int settings_trunk_id = 0;
    for (int i = 0; i < serviceTrunkSettings->size(); i++) {

        ServiceTrunkSettings *settings = serviceTrunkSettings->get(i);
        if (settings->id == trunk_settings_id) {

            trunkSettings = *settings;
            settings_trunk_id = settings->trunk_id;
            break;
        }
    }
    if (settings_trunk_id == 0)
        return nullptr;

    int trunk_id = 0;
    for (int i = 0; i < serviceTrunk->size(); i++) {

        ServiceTrunk *st = serviceTrunk->get(i);
        if (st->id == settings_trunk_id) {

            trunk_id = st->trunk_id;
            break;
        }
    }
    if (trunk_id == 0)
        return nullptr;

    return this->getTrunk(trunk_id);
}

Trunk *Repository::getTrunkByName(const char *trunk_name) {

    if (trunk_name==nullptr || (strlen(trunk_name)==0))
        return nullptr;

    Trunk *trunk = trunkByName->find(trunk_name, trace);
    if (trunk != nullptr) {
        return trunk;
    }

    trunk = trunkByAlias->find(trunk_name, trace);
    if (trunk != nullptr) {
        return trunk;
    }

    return nullptr;
}

double Repository::priceToRoubles(double price, const Pricelist &pricelist) const {
    double currencyRate = 1.0;
    if (this->getCurrencyRate(pricelist.currency_id, &currencyRate)
        && currencyRate > 0.00000001) {
        return price * currencyRate;
    } else {
        return price;
    }
}

bool Repository::getCurrencyRate(const char *currency_id, double *o_currencyRate) const {
    if (!currency_id || !o_currencyRate) {
        throw Exception("Invalid arguments passed into getCurrencyRate()");
    }

    const CurrencyRate *rate = this->currencyRate->find(currency_id);
    if (rate) {
        *o_currencyRate = rate->rate;
        return true;
    } else {
        // "Безопасный" курс - 1:1
        *o_currencyRate = 1.0;
        return false;
    }
}

bool Repository::matchNumber(int number_id, long long int numberPrefix) {
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

bool Repository::priceLessThan(double priceLeft, const Pricelist &pricelistLeft,
                               double priceRight, const Pricelist &pricelistRight) const {
    if (0 == strncmp(pricelistLeft.currency_id, pricelistRight.currency_id, 4)) {
        return priceLeft < priceRight;
    } else {
        double leftRoubles = this->priceToRoubles(priceLeft, pricelistLeft);
        double rightRoubles = this->priceToRoubles(priceRight, pricelistRight);
        return leftRoubles < rightRoubles;
    }
}


bool Repository::getNNPDestinationByNumberRange(set<int> &nnpDestinationIds, NNPNumberRange *nnpNumberRange,
                                                stringstream *trace) {

    bool fResult = false;

    if (nnpNumberRange != nullptr) {
        vector<int> nnpPrefixIds;
        getNNPPrefixsByNumberRange(nnpPrefixIds, nnpNumberRange->id, trace);
        fResult = getNNPDestinationsByPrefix(nnpDestinationIds, nnpPrefixIds, trace);
    }
    if (trace != nullptr) {

        *trace << "INFO|NNP|NNP DESTINATION SET (";
        for (auto it = nnpDestinationIds.begin(); it != nnpDestinationIds.end(); it++) {
            NNPDestination *nnpDestination = getNNPDestination(*it);
            if (nnpDestination != nullptr) nnpDestination->dump(*trace);
            *trace << " ";
        }
        *trace << ")\n";
    }

//    for (auto it = nnpDestinationIds.begin(); it != nnpDestinationIds.end(); it++)
//        NNPDestination *nnpDestination = getNNPDestination(*it, trace);

    return fResult;
}

bool Repository::getNNPDestinationByNum(set<int> &nnpDestinationIds, long long int num, stringstream *trace) {
    bool fResult = false;

    NNPNumberRange *nnpNumberRange = getNNPNumberRange(num, trace);
    fResult = getNNPDestinationByNumberRange(nnpDestinationIds, nnpNumberRange, trace);
    if (trace != nullptr && !fResult) {
        *trace << "NOT FOUND|NNPDestination|BY num '" << num << "'" << "\n";

    }
    return fResult;
}

void Repository::getTrunkSettingsOrderList(vector<ServiceTrunkOrder> &resultTrunkSettingsTrunkOrderList, Trunk *trunk,
                                           long long int srcNumber, long long int dstNumber, int destinationType) {
    vector<ServiceTrunk *> serviceTrunks;
    getAllServiceTrunk(serviceTrunks, trunk->id);

    if (serviceTrunks.size() == 0) {
        if (trace != nullptr) {
            *trace << "DEBUG|SERVICE TRUNK DECLINE|CAUSE SERVICE TRUNK NOT FOUND BY TRUNK " << trunk->name <<
            " (" << trunk->id << ")" << "\n";
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
                        *trace << "DEBUG|TRUNK SETTINGS SKIP|ACCOUNT NOT FOUND BY ID " <<
                        serviceTrunk->client_account_id << "\n";
                    }
                    continue;
                }
                ServiceTrunkOrder order;
                order.trunk = trunk;
                order.account = account;
                order.serviceTrunk = serviceTrunk;
                order.trunkSettings = trunkSettings;
                order.statsTrunkSettings = nullptr;
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

        for (auto order : resultTrunkSettingsTrunkOrderList) {
            if (trace != nullptr) {
                *trace << "||";
                order.dump(*trace);
                *trace << "\n";
            }
        }
    } else {
        if (trace != nullptr) {
            *trace << "NOT FOUND|TRUNK SETTING ORDER LIST|" << "\n";
        }
    }
}

bool Repository::checkTrunkSettingsConditions(ServiceTrunkSettings *&trunkSettings, long long int srcNumber,
                                              long long int dstNumber, Pricelist *&pricelist, PricelistPrice *&price) {

    if (trunkSettings->src_number_id > 0 && !matchNumber(trunkSettings->src_number_id, srcNumber)) {
        if (trace != nullptr) {
            *trace << "DEBUG|TRUNK SETTINGS DECLINE|BY SRC NUMBER MATCHING, TRUNK_SETTINGS_ID: " <<
            trunkSettings->id << " / " << trunkSettings->order << "\n";
        }
        return false;
    }

    if (trunkSettings->dst_number_id > 0 && !matchNumber(trunkSettings->dst_number_id, dstNumber)) {
        if (trace != nullptr) {
            *trace << "DEBUG|TRUNK SETTINGS DECLINE|BY DST NUMBER MATCHING, TRUNK_SETTINGS_ID: " <<
            trunkSettings->id << " / " << trunkSettings->order << "\n";
        }
        return false;
    }

    pricelist = getPricelist(trunkSettings->pricelist_id);
    if (pricelist == nullptr) {
        if (trace != nullptr) {
            *trace << "DEBUG|TRUNK SETTINGS DECLINE|PRICELIST NOT FOUND BY ID: " << trunkSettings->pricelist_id <<
            ", TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << "\n";
        }
        return false;
    }

    if (pricelist->local) {

        auto networkPrefix = getNetworkPrefix(pricelist->local_network_config_id, dstNumber);
        if (networkPrefix == nullptr) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK SETTINGS DECLINE|NETWORK PREFIX NOT FOUND BY local_network_config_id: " <<
                pricelist->local_network_config_id << ", PRICELIST_ID: " << pricelist->id <<
                ", TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " << trunkSettings->order << " / " << "\n";
            }
            return false;
        }

        price = getPrice(trunkSettings->pricelist_id, networkPrefix->network_type_id);
        if (price == nullptr) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK SETTINGS DECLINE|PRICE NOT FOUND: PRICELIST_ID: " << pricelist->id <<
                ", PREFIX: " << networkPrefix->network_type_id << ", TRUNK_SETTINGS_ID: " << trunkSettings->id <<
                " / " << trunkSettings->order << "\n";
            }
            return false;
        }


    } else {

        price = getPrice(trunkSettings->pricelist_id, dstNumber);
        if (price == nullptr) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK SETTINGS DECLINE|PRICE NOT FOUND: PRICELIST_ID: " << pricelist->id <<
                ", PREFIX: " << dstNumber << ", TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " <<
                trunkSettings->order << "\n";
            }
            return false;
        }

    }

    if (trace != nullptr) {
        *trace << "DEBUG|TRUNK SETTINGS ACCEPT|TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " <<
        trunkSettings->order << "\n";
    }

    return true;
}

bool Repository::trunkOrderLessThan(const ServiceTrunkOrder &left, const ServiceTrunkOrder &right) const {
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

double Repository::getVatRate(Client *client) {
    if (client != nullptr && !client->price_include_vat) {
        auto org = organization->find(client->organization_id, time(nullptr));
        if (org != nullptr) {
            return org->vat_rate;
        }
    }
    return 0;
}

bool Repository::matchPrefixlist(int prefixlist_id, char *prefix) {
    auto prefixlist = getPrefixlist(prefixlist_id);
    if (prefixlist == nullptr) {
        return false;
    }

    auto prefixlistPrefix = getPrefixlistPrefix(prefixlist->id, prefix);
    return prefixlistPrefix != nullptr;
}

PhoneNumber
Repository::getNNPBestGeoRoute(PhoneNumber NumAdef, vector<PhoneNumber> &vNumA, PhoneNumber NumB, stringstream *trace) {

    return -1;
}

void Repository::getNNPBestPriceRoute(vector<pair<double, PhoneNumber>> &vResNum, vector<PhoneNumber> &vNumA,
                                      PhoneNumber NumB, stringstream *trace) {
    vResNum.push_back(make_pair(1.23, 79263747216));
    vResNum.push_back(make_pair(1.43, 73424234424));
}

