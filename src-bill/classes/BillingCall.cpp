#include "BillingCall.h"
#include "../classes/AppBill.h"

BillingCall::BillingCall(Billing *billing) {
    this->billing = billing;
    this->trace = nullptr;
}

void BillingCall::setTrace(stringstream *trace) {
    this->trace = trace;
}

void BillingCall::calc(Call *call, Cdr *cdr, PreparedData *preparedData) {
    this->call = call;
    this->cdr = cdr;
    this->data = preparedData;

    if (call->orig) {
        call->src_number = atoll(cdr->src_number);
        call->dst_number = atoll(cdr->dst_number);
    }

    trunk = data->trunkByName->find(getRoute(), trace);
    if (trunk == nullptr) {
        trunk = data->trunkByAlias->find(getRoute(), trace);
        if (trunk == nullptr) {
            if (trace != nullptr) {
                *trace << "ERROR|TARIFFICATION STOPPED|CAUSE TRUNK WAS NOT FOUND" << endl;
            }
            return;
        }
    }
    call->trunk_id = trunk->id;

    if (call->orig) {
        numberPreprocessing();
    }

    fillGeoPrefix();

    if (trunk->auth_by_number) {
        calcByNumber();
    } else {
        calcByTrunk();
    }
}

void BillingCall::fillGeoPrefix() {

    GeoPrefix * geoPrefix;
    if (call->orig) {
        geoPrefix = data->geoPrefix->find(call->dst_number, trace);
    } else {
        geoPrefix = data->geoPrefix->find(call->src_number, trace);
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

    bool srcNumberPreprocessingDone = false;
    bool dstNumberPreprocessingDone = false;
    while (true) {
        auto numberPreprocessing = data->trunkNumberPreprocessing->find(trunk->id, order, trace);
        if (numberPreprocessing == nullptr) {
            break;
        }

        if (!srcNumberPreprocessingDone && numberPreprocessing->src && cdr->src_noa == numberPreprocessing->noa && strlen(cdr->src_number) == numberPreprocessing->length) {
            char tmpNumber[20];
            char tmp[20];

            sprintf(tmpNumber, "%lld", call->src_number);
            strcpy(tmp, numberPreprocessing->prefix);
            strcat(tmp, tmpNumber);

            call->src_number = atoll(tmp);

            if (trace != nullptr) {
                *trace << "INFO|NUMBER PREPROCESSING|SET SRC_NUMBER = " << call->src_number << endl;
            }
            srcNumberPreprocessingDone = true;
        }

        if (!dstNumberPreprocessingDone && !numberPreprocessing->src && cdr->dst_noa == numberPreprocessing->noa && strlen(cdr->dst_number) == numberPreprocessing->length) {
            char tmpNumber[20];
            char tmp[20];

            sprintf(tmpNumber, "%lld", call->dst_number);
            strcpy(tmp, numberPreprocessing->prefix);
            strcat(tmp, tmpNumber);

            call->dst_number = atoll(tmp);

            if (trace != nullptr) {
                *trace << "INFO|NUMBER PREPROCESSING|SET DST_NUMBER = " << call->dst_number << endl;
            }
            dstNumberPreprocessingDone = true;
        }

        order++;
    }
}

int BillingCall::getDest(GeoPrefix * geoPrefix) {
    if (geoPrefix->geo_id == data->instanceSettings->city_geo_id) {
        if (trace != nullptr) {
            *trace << "INFO|GEO_PREFIX SET DEST = -1|CAUSE GEO_ID = INSTANCE_SETTINGS->CITY_GEO_ID" << endl;
        }
        return -1;
    } else {
        auto regionIds = data->instanceSettings->getRegionIds();
        for (auto it = regionIds.begin(); it != regionIds.end(); ++it) {
            if (geoPrefix->region_id == *it) {
                if (trace != nullptr) {
                    *trace << "INFO|GEO_PREFIX SET DEST = 0|CAUSE REGION_ID IN INSTANCE_SETTINGS->REGION_IDS" << endl;
                }
                return 0;
            }
        }
        return geoPrefix->dest;
    }
}

void BillingCall::calcByTrunk() {

    if (trace != nullptr) {
        *trace << "INFO|TARIFFICATION BY TRUNK" << endl;
    }

    vector<ServiceTrunk *> serviceTrunks;
    data->serviceTrunk->findAll(serviceTrunks, trunk->id, call->connect_time, trace);
    if (serviceTrunks.size() == 0) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE SERVICE TRUNK WAS NOT FOUND" << endl;
        }
        return;
    }

    ServiceTrunk * effectiveServiceTrunk = nullptr;
    Pricelist * effectivePricelist = nullptr;
    PricelistPrice * effectivePrice = nullptr;
    for (auto serviceTrunk : serviceTrunks) {
        Pricelist * pricelist;
        PricelistPrice * price;
        if (checkServiceTrunkAvailability(serviceTrunk, call->orig ? SERVICE_TRUNK_SETTINGS_ORIGINATION : SERVICE_TRUNK_SETTINGS_TERMINATION, pricelist, price)) {
            if (effectiveServiceTrunk == nullptr || price->price < effectivePrice->price) {
                effectiveServiceTrunk = serviceTrunk;
                effectivePricelist = pricelist;
                effectivePrice = price;
            }
        }
    }

    if (effectiveServiceTrunk == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE EFFECTIVE SERVICE TRUNK WAS NOT FOUND" << endl;
        }
        return;
    }

    if (trace != nullptr) {
        *trace << "INTO|EFFECTIVE PRICELIST|";
        effectivePricelist->dump(*trace);
        *trace << endl;

        *trace << "INTO|EFFECTIVE PRICELIST PRICE|";
        effectivePrice->dump(*trace);
        *trace << endl;
    }

    call->account_id = effectiveServiceTrunk->client_account_id;
    call->trunk_service_id = effectiveServiceTrunk->id;
    call->operator_id = effectiveServiceTrunk->operator_id;

    call->pricelist_id = effectivePricelist->id;
    call->prefix = atoll(effectivePrice->prefix);

    call->billed_time = getCallLength(
            cdr->session_time,
            effectivePricelist->tariffication_by_minutes,
            effectivePricelist->tariffication_full_first_minute,
            false
    );

    call->rate = effectivePrice->price;
    call->cost = call->billed_time * call->rate / 60.0;
    call->cost = call->orig ? - call->cost : call->cost;
}

void BillingCall::calcByNumber() {

    if (trace != nullptr) {
        *trace << "INFO|TARIFFICATION BY NUMBER" << endl;
    }

    if (call->orig) {
        processLineWithoutNumber(call, cdr);
    }

    auto serviceNumber = data->serviceNumber->find(getNumber(), call->connect_time, trace);
    if (serviceNumber == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE SERVICE NUMBER WAS NOT FOUND" << endl;
        }
        return;
    }

    auto serviceTrunk = data->serviceTrunk->find(trunk->id, call->connect_time, trace);
    if (serviceTrunk != nullptr) {
        call->trunk_service_id = serviceTrunk->id;
        call->operator_id = serviceTrunk->operator_id;
    }

    if (!trunk->our_trunk) {

        if (serviceTrunk == nullptr) {
            if (trace != nullptr) {
                *trace << "ERROR|TARIFFICATION STOPPED|CAUSE SERVICE TRUNK WAS NOT FOUND" << endl;
            }
            return;
        }

        if (serviceNumber->client_account_id != serviceTrunk->client_account_id) {
            if (trace != nullptr) {
                *trace << "ERROR|TARIFFICATION STOPPED|CAUSE SERVICE NUMBER ACCOUNT DIFFERENT FROM SERVICE TRUNK_ACCOUNT" << endl;
            }
            return;
        }
    }



    call->account_id = serviceNumber->client_account_id;
    call->number_service_id = serviceNumber->id;

    if (call->orig) {
        calcOrigByNumber(serviceNumber);
    } else {
        calcTermByNumber(serviceNumber);
    }
}

bool BillingCall::checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price) {

    vector<ServiceTrunkSettings *> serviceTrunkSettings;
    data->serviceTrunkSettings->findAll(serviceTrunkSettings, serviceTrunk->id, type, trace);
    for (auto trunkSettings : serviceTrunkSettings) {

        if (trunkSettings->src_number_id > 0 && !filterByNumber(trunkSettings->src_number_id, call->src_number)) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE SRC_NUMBER NOT MATCHED" << endl;
            }
            continue;
        }

        if (trunkSettings->dst_number_id > 0 && !filterByNumber(trunkSettings->dst_number_id, call->dst_number)) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE DST_NUMBER NOT MATCHED" << endl;
            }
            continue;
        }

        pricelist = data->pricelist->find(trunkSettings->pricelist_id, trace);
        if (pricelist == nullptr) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE PRICELIST NOT FOUND" << endl;
            }
            continue;
        }

        price = data->pricelistPrice->find(trunkSettings->pricelist_id, call->src_number, call->connect_time, trace);
        if (price == nullptr) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE PRICELIST PRICE NOT FOUND" << endl;
            }
            continue;
        }

        if (trace != nullptr) {
            *trace << "INFO|SERVICE TRUNK SETTINGS MATCHED" << endl;
        }

        return true;
    }

    return false;
}

void BillingCall::calcOrigByNumber(ServiceNumber *serviceNumber) {

    auto logTariff = data->tariffChangeLog->find(serviceNumber->id, call->connect_time, trace);
    if (logTariff == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE TARIFF CHANGE LOG NOT FOUND" << endl;
        }
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


    auto tariff = data->tariff->find(tariffId, trace);
    if (tariff == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE TARIFF NOT FOUND" << endl;
        }
        return;
    }

    call->pricelist_id = tariff->pricelist_id;

    auto price = data->pricelistPrice->find(call->pricelist_id, call->dst_number, call->connect_time, trace);
    if (price == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE PRICELIST PRICE NOT FOUND" << endl;
        }
        return;
    }

    call->prefix = atoll(price->prefix);

    call->billed_time = getCallLength(
            cdr->session_time,
            tariff->tariffication_by_minutes,
            tariff->tariffication_full_first_minute,
            tariff->tariffication_free_first_seconds
    );

    call->rate = price->price;
    call->cost = - (call->billed_time * call->rate / 60.0);

    int freeSeconds = 60 * tariff->freemin * (tariff->freemin_for_number ? 1 : serviceNumber->lines_count);
    if (call->isLocal() && freeSeconds > 0) {
        auto fminCounter = billing->billingData->fminCounter.get();
        int used_free_seconds = fminCounter->get(call->number_service_id, 1, call->dt.month);
        if (used_free_seconds + call->billed_time <= freeSeconds) {
            call->service_package_id = 1;
            call->package_time = call->billed_time;
            call->package_credit = call->cost;
            call->cost = 0;
        }
    }
}

void BillingCall::calcTermByNumber(ServiceNumber *serviceNumber) {

    auto logTariff = data->tariffChangeLog->find(serviceNumber->id, call->connect_time, trace);
    if (logTariff == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE TARIFF CHANGE LOG NOT FOUND" << endl;
        }
        return;
    }

    auto tariff = data->tariff->find(logTariff->tariff_id_local, trace);
    if (tariff == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE TARIFF NOT FOUND" << endl;
        }
        return;
    }

    call->billed_time = getCallLength(
            cdr->session_time,
            tariff->tariffication_by_minutes,
            tariff->tariffication_full_first_minute,
            tariff->tariffication_free_first_seconds
    );

    if (isUsage7800()) {
        if (trace != nullptr) {
            *trace << "INFO|TARIFFICATION 7800" << endl;
        }

        call->pricelist_id = tariff->pricelist_id;

        auto price = data->pricelistPrice->find(call->pricelist_id, call->src_number, call->connect_time, trace);
        if (price == nullptr) {
            if (trace != nullptr) {
                *trace << "ERROR|TARIFFICATION STOPPED|CAUSE PRICELIST PRICE NOT FOUND" << endl;
            }
            return;
        }

        call->prefix = atoll(price->prefix);

        call->rate = price->price;
        call->cost = call->billed_time * call->rate / 60.0;
    }
}

void BillingCall::processLineWithoutNumber(Call *call, Cdr *cdr) {
    char * pos = nullptr;
    if (pos == nullptr) {
        pos = strstr(cdr->src_number, "=2A");
        if (pos != nullptr) pos += 3;
    }
    if (pos == nullptr) {
        pos = strstr(cdr->src_number, "=2B");
        if (pos != nullptr) pos += 3;
    }
    if (pos == nullptr) {
        pos = strstr(cdr->src_number, "*");
        if (pos != nullptr) pos += 1;
    }
    if (pos == nullptr) {
        pos = strstr(cdr->src_number, "+");
        if (pos != nullptr) pos += 1;
    }
    if (pos != nullptr) {
        call->src_number = atoll(pos);
        if (trace != nullptr) {
            *trace << "INFO|LINE WITHOUT NUMBER|SET SRC_NUMBER = " << call->src_number << endl;
        }
    }
}

long long int BillingCall::getNumber() {
    return call->orig ? call->src_number : call->dst_number;
}

long long int BillingCall::getRemoteNumber() {
    return !call->orig ? call->dst_number : call->src_number;
}

char * BillingCall::getRoute() {
    return call->orig ? cdr->src_route : cdr->dst_route;
}

char * BillingCall::getRemoteRoute() {
    return call->orig ? cdr->dst_route : cdr->src_route;
}

bool BillingCall::filterByNumber(int numberId, long long int numberPrefix) {
    char tmpNumber[20];
    sprintf(tmpNumber, "%lld", numberPrefix);

    auto number = data->number->find(numberId);
    if (number == nullptr) {
        return false;
    }

    auto prefixlistIds = number->getPrefixlistIds();
    for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {
        if (filterByPrefixlist(*it, tmpNumber)) {
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
    char tmpNumber[20];
    sprintf(tmpNumber, "%lld", getNumber());

    return tmpNumber[0] == '7' && tmpNumber[1] == '8' && tmpNumber[2] == '0' && tmpNumber[3] == '0';
}