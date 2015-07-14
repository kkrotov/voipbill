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

    if ((servicePackage = data->servicePackage.get()) == nullptr) {
        return false;
    }

    if ((serviceTrunk = data->serviceTrunk.get()) == nullptr) {
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

    if ((activeCounter = data->activeCounter.get()) == nullptr) {
        return false;
    }

    return true;
}

