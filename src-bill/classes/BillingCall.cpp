#include "BillingCall.h"
#include "../classes/AppBill.h"

BillingCall::BillingCall(Billing *billing) {
    this->billing = billing;
}

void BillingCall::calc(Call *call, Cdr *cdr, PreparedData *preparedData) {
    this->call = call;
    this->cdr = cdr;
    this->data = preparedData;

    trunk = data->trunk->find(getRoute());
    if (trunk == nullptr) {
        return;
    }

    numberPreprocessing();

    fillGeoPrefix();


    if (trunk->our_trunk) {
        calcByNumber();
    } else {
        calcByTrunk();
    }
}

void BillingCall::fillGeoPrefix() {

    GeoPrefix * geoPrefix;
    if (call->orig) {
        geoPrefix = data->geoPrefix->find(call->dst_number);
    } else {
        geoPrefix = data->geoPrefix->find(call->src_number);
    }

    if (geoPrefix != nullptr) {
        call->mob = geoPrefix->mob;
        call->dest = getDest(geoPrefix);
        call->geo_id = geoPrefix->geo_id;
        call->geo_operator_id = geoPrefix->geo_operator_id;;
    }
}


void BillingCall::numberPreprocessing() {
    int order = 1;

    while (true) {
        auto numberPreprocessing = data->trunkNumberPreprocessing->find(trunk->id, order);
        if (numberPreprocessing == nullptr) {
            break;
        }

        if (numberPreprocessing->src && cdr->src_noa == numberPreprocessing->noa && strlen(cdr->src_number) == numberPreprocessing->length) {
            strcpy(call->src_number, numberPreprocessing->prefix);
            strcat(call->src_number, cdr->src_number);
            break;
        }

        if (!numberPreprocessing->src && cdr->dst_noa == numberPreprocessing->noa && strlen(cdr->dst_number) == numberPreprocessing->length) {
            strcpy(call->dst_number, numberPreprocessing->prefix);
            strcat(call->src_number, cdr->dst_number);
            break;
        }

        order++;
    }
}

int BillingCall::getDest(GeoPrefix * geoPrefix) {
    if (geoPrefix->geo_id == data->instanceSettings->city_geo_id) {
        return -1;
    } else {
        auto regionIds = data->instanceSettings->getRegionIds();
        for (auto it = regionIds.begin(); it != regionIds.end(); ++it) {
            if (geoPrefix->region_id == *it) {
                return 0;
            }
        }
        return geoPrefix->dest;
    }
}

void BillingCall::calcByTrunk() {
    auto serviceTrunk = data->serviceTrunk->find(getRoute(), time(NULL));
    if (serviceTrunk == nullptr) {
        return;
    }

    call->service_type = SERVICE_TYPE_TRUNK;
    call->service_id = serviceTrunk->id;
    call->client_account_id = serviceTrunk->client_account_id;

    if (call->orig) {
        calcOrigByTrunk(serviceTrunk);
    } else {
        calcTermByTrunk(serviceTrunk);
    }
}

void BillingCall::calcByNumber() {
    auto serviceNumber = data->serviceNumber->find(getNumber(), time(NULL));
    if (serviceNumber == nullptr) {
        return;
    }

    call->service_type = SERVICE_TYPE_NUMBER;
    call->service_id = serviceNumber->id;
    call->client_account_id = serviceNumber->client_account_id;

    if (call->orig) {
        calcOrigByNumber(serviceNumber);
    } else {
        calcTermByNumber(serviceNumber);
    }
}

void BillingCall::calcOrigByTrunk(ServiceTrunk *serviceTrunk) {
    int order = 1;
    while (true) {
        auto trunkSettings = data->serviceTrunkSettings->find(serviceTrunk->id, true, order);
        if (trunkSettings == nullptr) {
            return;
        }
        order++;

        if (trunkSettings->src_number_id > 0 && !filterByNumber(trunkSettings->src_number_id, call->src_number)) {
            continue;
        }

        if (trunkSettings->dst_number_id > 0 && !filterByNumber(trunkSettings->dst_number_id, call->dst_number)) {
            continue;
        }

        auto pricelist = data->pricelist->find(trunkSettings->pricelist_id);
        if (pricelist == nullptr) {
            continue;
        }

        auto price = data->pricelistPrice->find(trunkSettings->pricelist_id, call->src_number, time(NULL));
        if (price == nullptr) {
            continue;
        }

        call->billed_time = getCallLength(
                cdr->session_time,
                pricelist->tariffication_by_minutes,
                pricelist->tariffication_full_first_minute,
                false
        );

        call->rate = price->price;
        call->cost = call->billed_time * call->rate / 60.0;
        return;
    }
}

void BillingCall::calcTermByTrunk(ServiceTrunk *serviceTrunk) {
    int order = 1;
    while (true) {
        auto trunkSettings = data->serviceTrunkSettings->find(serviceTrunk->id, false, order);
        if (trunkSettings == nullptr) {
            return;
        }
        order++;

        if (trunkSettings->src_number_id > 0 && !filterByNumber(trunkSettings->src_number_id, call->src_number)) {
            continue;
        }

        if (trunkSettings->dst_number_id > 0 && !filterByNumber(trunkSettings->dst_number_id, call->dst_number)) {
            continue;
        }

        auto pricelist = data->pricelist->find(trunkSettings->pricelist_id);
        if (pricelist == nullptr) {
            continue;
        }

        auto price = data->pricelistPrice->find(trunkSettings->pricelist_id, call->src_number, time(NULL));
        if (price == nullptr) {
            continue;
        }

        call->billed_time = getCallLength(
                cdr->session_time,
                pricelist->tariffication_by_minutes,
                pricelist->tariffication_full_first_minute,
                false
        );

        call->rate = -price->price;
        call->cost = call->billed_time * call->rate / 60.0;
        return;
    }
}

void BillingCall::calcOrigByNumber(ServiceNumber *serviceNumber) {
    auto logTariff = data->tariffChangeLog->find(serviceNumber->id, time(NULL));
    if (logTariff == nullptr) {
        return;
    }

    int tariffId = 0;
    if (call->isLocal())
        tariffId = logTariff->tariff_id_local;
    else if (call->isZonaMob())
        tariffId = logTariff->tariff_id_local_mob;
    else if (call->isZonaStd())
        tariffId = logTariff->tariff_id_russia;
    else if (call->isRussianStd())
        tariffId = logTariff->tariff_id_russia;
    else if (call->isRussianMob())
        tariffId = logTariff->tariff_id_russia_mob;
    else if (call->isInternational())
        tariffId = logTariff->tariff_id_intern;
    else if (call->isSNG())
        tariffId = logTariff->tariff_id_sng;


    auto tariff = data->tariff->find(tariffId);
    if (tariff == nullptr) {
        return;
    }

    call->pricelist_id = tariff->pricelist_id;

    auto price = data->pricelistPrice->find(call->pricelist_id, call->dst_number, time(NULL));
    if (price == nullptr) {
        return;
    }

    call->billed_time = getCallLength(
            cdr->session_time,
            tariff->tariffication_by_minutes,
            tariff->tariffication_full_first_minute,
            tariff->tariffication_free_first_seconds
    );

    call->rate = price->price;
    call->cost = call->billed_time * call->rate / 60.0;
}

void BillingCall::calcTermByNumber(ServiceNumber *serviceNumber) {
    auto logTariff = data->tariffChangeLog->find(serviceNumber->id, time(NULL));
    if (logTariff == nullptr) {
        return;
    }

    auto tariff = data->tariff->find(logTariff->tariff_id_local);
    if (tariff == nullptr) {
        return;
    }

    call->billed_time = getCallLength(
            cdr->session_time,
            tariff->tariffication_by_minutes,
            tariff->tariffication_full_first_minute,
            tariff->tariffication_free_first_seconds
    );

    if (isUsage7800()) {
        call->pricelist_id = tariff->pricelist_id;

        auto price = data->pricelistPrice->find(call->pricelist_id, call->src_number, time(NULL));
        if (price == nullptr) {
            return;
        }

        call->rate = -price->price;
        call->cost = call->billed_time * call->rate / 60.0;
    }
}

char * BillingCall::getNumber() {
    return call->orig ? call->src_number : call->dst_number;
}

char * BillingCall::getRemoteNumber() {
    return !call->orig ? call->dst_number : call->src_number;
}

char * BillingCall::getRoute() {
    return call->orig ? cdr->src_route : cdr->dst_route;
}

char * BillingCall::getRemoteRoute() {
    return call->orig ? cdr->dst_route : cdr->src_route;
}

bool BillingCall::filterByNumber(int numberId, char * str) {
    auto number = data->number->find(numberId);
    if (number == nullptr) {
        return false;
    }

    auto prefixlistIds = number->getPrefixlistIds();
    for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {
        if (filterByPrefixlist(*it, str)) {
            return true;
        }
    }

    return false;

}

bool BillingCall::filterByPrefixlist(int prefixlistId, char * str) {
    auto prefixlist = data->prefixlist->find(prefixlistId);
    if (prefixlist == nullptr) {
        return false;
    }

    auto prefix = data->prefixlistPrefix->find(prefixlist->id, str);
    return prefix != nullptr;
}

int BillingCall::getCallLength(int len, bool byMinutes, bool fullFirstMinute, bool freeFirstMinutes) {

    if (freeFirstMinutes && len <= app().conf.billing_free_seconds)
        return 0;

    if (len <= 0)
        return 0;

    if (byMinutes) {

        int s = len % 60;
        if (s == 0)
            return len;

        return len + 60 - s;

    } else {

        if (fullFirstMinute && len < 60)
            return 60;

        return len;

    }
}

bool BillingCall::isUsage7800() {
    char *pNumber = getNumber();
    return *pNumber++ == '7' && *pNumber++ == '8' && *pNumber++ == '0' && *pNumber++ == '0';
}