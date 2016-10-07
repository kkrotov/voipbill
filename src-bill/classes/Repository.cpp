#include "Repository.h"

bool Repository::prepare(time_t currentTime) {

    this->currentTime = currentTime == 0 ? time(nullptr) : currentTime;

    {
        shared_ptr<ServerList> list = data->server.get();
        if (list != nullptr) {
            auto item = list->find(app().conf.instance_id);
            if (item != nullptr) {
                server = data->server.get()->find(app().conf.instance_id);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    {
        shared_ptr<InstanceSettingsList> list = data->instanceSettings.get();
        if (list != nullptr) {
            auto item = list->find(app().conf.instance_id);
            if (item != nullptr) {
                instanceSettings = item;
            } else {
                return false;
            }
        } else {
            return false;
        }
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

void Repository::orderTermTrunkSettingsOrderList(vector<ServiceTrunkOrder> &trunkSettingsOrderList,
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

        if (order.trunkSettings->minimum_minutes > 0) {
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

        if (order.trunkSettings->minimum_cost > 0) {
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

    sort(trunkSettingsOrderFreeList.begin(), trunkSettingsOrderFreeList.end(), trunk_settings_order_asc_price(*this));
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