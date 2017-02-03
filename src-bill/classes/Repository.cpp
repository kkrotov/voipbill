#include "Repository.h"


bool Repository::prepare(time_t currentTime) {

    this->currentTime = currentTime == 0 ? time(nullptr) : currentTime;
    notReadyObj = "";

    if ((hub = data->hub.get()) == nullptr) {
        notReadyObj = "HubList";
        return false;
    }
    if (app().conf.hub_id > 0) {
        auto item = hub->find(app().conf.hub_id);
        if (item == nullptr)
            return false;
    }

    if ((server = data->server.get()) == nullptr) {
        notReadyObj = "ServerList";
        return false;
    }
    {
        auto item = server->find(app().conf.instance_id);
        if (item == nullptr) {

            notReadyObj = "instance_id="+app().conf.instance_id;
            return false;
        }
    }

    if ((instanceSettings = data->instanceSettings.get()) == nullptr) {
        notReadyObj = "InstanceSettingsList";
        return false;
    }
    {
        auto item = instanceSettings->find(app().conf.instance_id);
        if (item == nullptr) {

            notReadyObj = "instance_id="+app().conf.instance_id;
            return false;
        }
    }

    if ((airp = data->airp.get()) == nullptr) {
        notReadyObj = "AirpList";
        return false;
    }

    if ((number = data->number.get()) == nullptr) {
        notReadyObj = "NumberList";
        return false;
    }

    if ((outcome = data->outcome.get()) == nullptr) {
        notReadyObj = "OutcomeList";
        return false;
    }

    if ((prefixlist = data->prefixlist.get()) == nullptr) {
        notReadyObj = "PrefixlistList";
        return false;
    }

    if ((prefixlistPrefix = data->prefixlistPrefix.get()) == nullptr) {
        notReadyObj = "PrefixlistPrefixList";
        return false;
    }

    if ((releaseReason = data->releaseReason.get()) == nullptr) {
        notReadyObj = "ReleaseReasonList";
        return false;
    }

    if ((routeCase = data->routeCase.get()) == nullptr) {
        notReadyObj = "RouteCaseList";
        return false;
    }

    if ((routeTable = data->routeTable.get()) == nullptr) {
        notReadyObj = "RouteTableList";
        return false;
    }

    if ((routeTableRoute = data->routeTableRoute.get()) == nullptr) {
        notReadyObj = "RouteTableRouteList";
        return false;
    }

    if ((trunk = data->trunk.get()) == nullptr) {
        notReadyObj = "TrunkList";
        return false;
    }

    if ((trunkGroup = data->trunkGroup.get()) == nullptr) {
        notReadyObj = "TrunkGroupList";
        return false;
    }

    if ((trunkGroupItem = data->trunkGroupItem.get()) == nullptr) {
        notReadyObj = "TrunkGroupItemList";
        return false;
    }

    if ((trunkByName = data->trunkByName.get()) == nullptr) {
        notReadyObj = "TrunkByNameList";
        return false;
    }

    if ((trunkByAlias = data->trunkByAlias.get()) == nullptr) {
        notReadyObj = "TrunkByAliasList";
        return false;
    }

    if ((trunkNumberPreprocessing = data->trunkNumberPreprocessing.get()) == nullptr) {
        notReadyObj = "TrunkNumberPreprocessingList";
        return false;
    }

    if ((trunkPriority = data->trunkPriority.get()) == nullptr) {
        notReadyObj = "TrunkPriorityList";
        return false;
    }

    if ((trunkRule = data->trunkRule.get()) == nullptr) {
        notReadyObj = "TrunkRuleList";
        return false;
    }

    if ((trunkTrunkRule = data->trunkTrunkRule.get()) == nullptr) {
        notReadyObj = "TrunkTrunkRuleList";
        return false;
    }

    if ((client = data->client.get()) == nullptr) {
        notReadyObj = "ClientList";
        return false;
    }

    if ((organization = data->organization.get()) == nullptr) {
        notReadyObj = "OrganizationList";
        return false;
    }

    if ((voipOperator = data->voipOperator.get()) == nullptr) {
        notReadyObj = "OperatorList";
        return false;
    }

    if ((pricelist = data->pricelist.get()) == nullptr) {
        notReadyObj = "PricelistList";
        return false;
    }

    if ((pricelistPrice = data->pricelistPrice.get()) == nullptr) {
        notReadyObj = "PricelistPriceList";
        return false;
    }

    if ((networkPrefix = data->networkPrefix.get()) == nullptr) {
        notReadyObj = "NetworkPrefixList";
        return false;
    }

    if ((geo = data->geo.get()) == nullptr) {
        notReadyObj = "GeoList";
        return false;
    }

    if ((mobPrefix = data->mobPrefix.get()) == nullptr) {
        notReadyObj = "MobPrefixList";
        return false;
    }

    if ((geoPrefix = data->geoPrefix.get()) == nullptr) {
        notReadyObj = "GeoPrefixList";
        return false;
    }

    if ((serviceNumber = data->serviceNumber.get()) == nullptr) {
        notReadyObj = "ServiceNumberList";
        return false;
    }

    if ((serviceNumberByTechNumber = data->serviceNumberByTechNumber.get()) == nullptr) {
        notReadyObj = "ServiceNumberByTechNumberList";
        return false;
    }

    if ((serviceNumberPackage = data->serviceNumberPackage.get()) == nullptr) {
        notReadyObj = "ServiceNumberPackageList";
        return false;
    }

    if ((serviceTrunk = data->serviceTrunk.get()) == nullptr) {
        notReadyObj = "ServiceTrunkList";
        return false;
    }

    if ((serviceTrunkPackage = data->serviceTrunkPackage.get()) == nullptr) {
        notReadyObj = "ServiceTrunkPackageList";
        return false;
    }

    if ((serviceTrunkSettings = data->serviceTrunkSettings.get()) == nullptr) {
        notReadyObj = "ServiceTrunkSettingsList";
        return false;
    }

    if ((tariff = data->tariff.get()) == nullptr) {
        notReadyObj = "TariffList";
        return false;
    }

    if ((tariffPackage = data->tariffPackage.get()) == nullptr) {
        notReadyObj = "TariffPackageList";
        return false;
    }

    if ((tariffChangeLog = data->tariffChangeLog.get()) == nullptr) {
        notReadyObj = "TariffChangeLogList";
        return false;
    }

    if ((statDestinationPrefixlists = data->statDestinationPrefixlists.get()) == nullptr) {
        notReadyObj = "StatDestinationPrefixlistsList";
        return false;
    }

    if ((statPrefixlist = data->statPrefixlist.get()) == nullptr) {
        notReadyObj = "StatPrefixlistList";
        return false;
    }

    if ((activeCounter = data->activeCounter.get()) == nullptr) {
        notReadyObj = "ActiveCounter";
        return false;
    }

    if ((currencyRate = data->currencyRate.get()) == nullptr) {
        notReadyObj = "CurrencyRateList";
        return false;
    }

    if ((nnpDestination = data->nnpDestination.get()) == nullptr) {
        notReadyObj = "NNPDestinationList";
        return false;
    }

    if ((nnpNumberRangePrefix = data->nnpNumberRangePrefix.get()) == nullptr) {
        notReadyObj = "NNPNumberRangePrefixList";
        return false;
    }

    if ((nnpPackage = data->nnpPackage.get()) == nullptr) {
        notReadyObj = "NNPPackageList";
        return false;
    }

    if ((nnpPackagePrice = data->nnpPackagePrice.get()) == nullptr) {
        notReadyObj = "NNPPackagePriceList";
        return false;
    }

    if ((nnpPrefix = data->nnpPrefix.get()) == nullptr) {
        notReadyObj = "NNPPrefixList";
        return false;
    }

    if ((nnpCountryCodeList = data->nnpCountry.get()) == nullptr) {
        notReadyObj = "NNPCountryCodeList";
        return false;
    }

    if ((nnpRegion = data->nnpRegion.get()) == nullptr) {
        notReadyObj = "NNPRegionList";
        return false;
    }

    if ((nnpNumberRange = data->nnpNumberRange.get()) == nullptr) {
        notReadyObj = "NNPNumberRangeList";
        return false;
    }

    if ((nnpOperator = data->nnpOperator.get()) == nullptr) {
        notReadyObj = "NNPOperatorList";
        return false;
    }

    if ((nnpPackageMinute = data->nnpPackageMinute.get()) == nullptr) {
        notReadyObj = "NNPPackageMinuteList";
        return false;
    }

    if ((nnpPackagePricelist = data->nnpPackagePricelist.get()) == nullptr) {
        notReadyObj = "NNPPackagePricelistList";
        return false;
    }

    if ((nnpPrefixDestination = data->nnpPrefixDestination.get()) == nullptr) {
        notReadyObj = "NNPPrefixDestinationList";
        return false;
    }

    if ((nnpAccountTariffLight = data->nnpAccountTariffLight.get()) == nullptr) {
        notReadyObj = "NNPAccountTariffLightList";
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

    stable_sort(trunkSettingsOrderList.begin(), trunkSettingsOrderList.end(), trunk_settings_order_asc_priority(*this));

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

    if (trunk_name == nullptr || (strlen(trunk_name) == 0))
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

double Repository::priceToRoubles(double price, const char *currency_id) const {
    double currencyRate = 1.0;
    if (this->getCurrencyRate(currency_id, &currencyRate)
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


void Repository::getTrunkSettingsOrderList(vector<ServiceTrunkOrder> &resultTrunkSettingsTrunkOrderList, Trunk *trunk,
                                           long long int srcNumber, long long int dstNumber, int destinationType) {

    set<int> nnpDestinationIds;

    NNPNumberRange *term_nnpNumberRange = getNNPNumberRange(dstNumber, trace);
    getNNPDestinationByNumberRange(nnpDestinationIds, term_nnpNumberRange);

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
            auto account = getAccount(serviceTrunk->client_account_id);
            if (account == nullptr) {
                if (trace != nullptr) {
                    *trace << "DEBUG|TRUNK SETTINGS SKIP|ACCOUNT NOT FOUND BY ID " <<
                           serviceTrunk->client_account_id << "\n";
                }
                continue;
            }

            if (account->account_version == CALL_ACCOUNT_VERSION_4) {
                Pricelist *pricelist;
                PricelistPrice *price;
                if (checkTrunkSettingsOldPricelistConditions(trunkSettings, srcNumber, dstNumber, pricelist, price)) {

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
            } else if (account->account_version == CALL_ACCOUNT_VERSION_5) {
                if (checkNNPTrunkSettingsConditions(trunkSettings, srcNumber, dstNumber)) {
                    set<pair<double, NNPPackagePrice *>> resultNNPPackagePriceIds;
                    set<pair<double, NNPPackagePricelist *>> resultNNPPackagePricelistIds;

                    findNNPPackagePriceIds(resultNNPPackagePriceIds, trunkSettings->nnp_tariff_id,
                                           nnpDestinationIds, trace);

                    findNNPPackagePricelistIds(resultNNPPackagePricelistIds, trunkSettings->nnp_tariff_id,
                                               dstNumber, trace);

                    if (resultNNPPackagePriceIds.size() == 0 && resultNNPPackagePricelistIds.size() == 0)
                        continue;

                    set<pair<double, pair<NNPPackagePricelist *, NNPPackagePrice *>>> resultNNPPackage;

                    for (auto i:resultNNPPackagePriceIds) {
                        if (i.second != nullptr)
                            resultNNPPackage.insert(make_pair(i.first, make_pair(nullptr, i.second)));
                    }

                    for (auto i:resultNNPPackagePricelistIds) {
                        if (i.second != nullptr)
                            resultNNPPackage.insert(make_pair(i.first, make_pair(i.second, nullptr)));
                    }


                    if (resultNNPPackage.size() == 0) {
                        if (trace != nullptr) {
                            *trace << "DEBUG|TRUNK SETTINGS DECLINE|NNP_PACKAGE NOT FOUND TRUNK_SETTINGS_ID: " <<
                                   trunkSettings->id << " / " << trunkSettings->order << "\n";
                        }
                        continue;
                    }

                    pair<NNPPackagePricelist *, NNPPackagePrice *> nnpPackagesPair = (*(resultNNPPackage.begin())).second;

                    ServiceTrunkOrder order;
                    order.trunk = trunk;
                    order.account = account;
                    order.serviceTrunk = serviceTrunk;
                    order.trunkSettings = trunkSettings;
                    order.statsTrunkSettings = nullptr;

                    if (nnpPackagesPair.first != nullptr) {
                        order.nnpPackagePricelist = nnpPackagesPair.first;
                    };

                    if (nnpPackagesPair.second != nullptr) {
                        order.nnpPackagePrice = nnpPackagesPair.second;
                    }

                    order.nnpPackage = getNNPPackage(trunkSettings->nnp_tariff_id, trace);

                    if (trace != nullptr) {
                        *trace << "DEBUG|TRUNK SETTINGS ACCEPT|TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " <<
                               trunkSettings->order << "\n";
                    }
                    order.nnp_price = (*(resultNNPPackage.begin())).first;
                    order.price = nullptr;
                    order.pricelist = nullptr;

                    resultTrunkSettingsTrunkOrderList.push_back(order);
                }
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

bool Repository::checkTrunkSettingsOldPricelistConditions(ServiceTrunkSettings *&trunkSettings, long long int srcNumber,
                                                          long long int dstNumber, Pricelist *&pricelist,
                                                          PricelistPrice *&price) {

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

void Repository::setCurrencyRate(Price &price) const {
    double currencyRate = 1.0;
    if (this->getCurrencyRate(price.currency.c_str(), &currencyRate)
        && currencyRate > 0.00000001) {
         price.currency_rate=currencyRate;
    } else {
         price.currency_rate=1;
    }
}

bool Repository::trunkOrderLessThan(const ServiceTrunkOrder &left, const ServiceTrunkOrder &right) const {

    Price leftPrice, rightPrice;

    if (left.account != nullptr) {
        if (left.nnpPackage != nullptr && left.account->account_version == CALL_ACCOUNT_VERSION_5) {
            leftPrice.set(left.nnp_price, left.nnpPackage->currency_id);
        }
        if (left.account->account_version == CALL_ACCOUNT_VERSION_4 && left.price && left.pricelist) {
            leftPrice.set(left.price->price, left.pricelist->currency_id);
        }
    }

    if (right.account != nullptr) {
        if (right.nnpPackage != nullptr && right.account->account_version == CALL_ACCOUNT_VERSION_5) {
            rightPrice.set(right.nnp_price, right.nnpPackage->currency_id);
        }
        if (right.account->account_version == CALL_ACCOUNT_VERSION_4 && right.price && right.pricelist) {
            rightPrice.set(right.price->price, right.pricelist->currency_id);
        }
    }

    if (leftPrice.currency != rightPrice.currency) {
        setCurrencyRate(leftPrice);
        setCurrencyRate(rightPrice);
    }

    return leftPrice < rightPrice;

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

void Repository::getTrunkPriority(int trunk_id, vector<TrunkPriority> &trunkPriorityList) {
    trunkPriority->findAllTrunkPriority(trunk_id, trunkPriorityList);
}

