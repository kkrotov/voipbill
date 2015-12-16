#include "BillingCall.h"
#include "../classes/AppBill.h"
#include "../classes/CalcException.h"

BillingCall::BillingCall(Repository *repository) {
    this->repository = repository;
    this->trace = nullptr;
}

void BillingCall::setTrace(stringstream *trace) {
    this->trace = trace;
    repository->trace = trace;
}

void BillingCall::clearTrace() {
    this->trace = nullptr;
    repository->trace = nullptr;
}

void BillingCall::calc(Call *call, CallInfo * callInfo, Cdr *cdr) {
    try {
        this->cdr = cdr;
        this->call = call;
        this->callInfo = callInfo;
        this->callInfo->call = call;

        setupTrunk();

        if (call->orig) {
            numberPreprocessing();
            processRedirectNumber();
        }

        processGeo();
        processDestinations();

        if (callInfo->trunk->auth_by_number) {
            calcByNumber();
        } else {
            calcByTrunk();
        }
    } catch (CalcException &e) {
        if (trace != nullptr) {
            *trace << "ERROR|TARIFFICATION STOPPED|CAUSE " << e.message << "\n";
        }
        return;
    }
}

void BillingCall::calcByTrunk() {

    if (trace != nullptr) {
        *trace << "INFO|TARIFFICATION BY TRUNK" << "\n";
    }

    setupEffectiveServiceTrunkPricelistPrice();

    setupServiceTrunk();

    setupAccount();

    setupPricelist();

    setupPrice();

    setupBilledTime();

    setupCost();

    if (!call->orig && callInfo->pricelist->initiate_zona_cost > 0.00001 && call->destination_id == 0) {
        call->interconnect_rate = callInfo->pricelist->initiate_zona_cost;
        call->interconnect_cost = call->billed_time * call->interconnect_rate / 60.0;
    }
    if (!call->orig && callInfo->pricelist->initiate_mgmn_cost > 0.00001 && call->destination_id > 0) {
        call->interconnect_rate = callInfo->pricelist->initiate_mgmn_cost;
        call->interconnect_cost = call->billed_time * call->interconnect_rate / 60.0;
    }
}

void BillingCall::calcByNumber() {

    if (trace != nullptr) {
        *trace << "INFO|TARIFFICATION BY NUMBER" << "\n";
    }

    if (call->orig) {
        processLineWithoutNumber(call, cdr);
    }

    setupServiceNumber();

    setupServiceTrunkForNumber();

    setupAccount();

    if (call->orig) {

        if (cdr->dst_replace[0] != 0) {
            for (auto srvNumber : repository->getServer()->service_numbers) {
                if (srvNumber.compare(cdr->dst_replace) == 0) {
                    call->is_service_number = true;
                    return;
                }
            }
        }

        calcOrigByNumber();
    } else {
        calcTermByNumber();
    }
}

void BillingCall::calcOrigByNumber() {

    setupLogTariff();

    setupMainTariff();

    setupBilledTime();

    setupPackagePricelist();

    if (callInfo->servicePackagePricelist == nullptr) {
        setupTariff();

        setupPricelist(callInfo->tariff->pricelist_id);

        setupPrice(call->dst_number);
    } else {
        setupPricelist();

        setupPrice();
    }

    setupFreemin();

    setupPackagePrepaid();

    setupCost();
}

void BillingCall::calcTermByNumber() {

    setupLogTariff();

    setupMainTariff();

    setupBilledTime();

    if (isUsage7800()) {
        if (trace != nullptr) {
            *trace << "INFO|TARIFFICATION 7800" << "\n";
        }

        setupPricelist(callInfo->mainTariff->pricelist_id);

        setupPrice(call->src_number);

        setupCost();

        call->cost = - call->cost;
    }
}


bool BillingCall::checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price) {

    vector<ServiceTrunkSettings *> serviceTrunkSettings;
    repository->getAllServiceTrunkSettings(serviceTrunkSettings, serviceTrunk->id, type);
    for (auto trunkSettings : serviceTrunkSettings) {

        if (trunkSettings->src_number_id > 0 && !repository->matchNumber(trunkSettings->src_number_id, call->src_number)) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE SRC_NUMBER NOT MATCHED" << "\n";
            }
            continue;
        }

        if (trunkSettings->dst_number_id > 0 && !repository->matchNumber(trunkSettings->dst_number_id, call->dst_number)) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE DST_NUMBER NOT MATCHED" << "\n";
            }
            continue;
        }

        pricelist = repository->getPricelist(trunkSettings->pricelist_id);
        if (pricelist == nullptr) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE PRICELIST NOT FOUND" << "\n";
            }
            continue;
        }

        if (pricelist->local) {

            auto networkPrefix = repository->getNetworkPrefix(pricelist->local_network_config_id, call->dst_number);
            if (networkPrefix == nullptr) {
                if (trace != nullptr) {
                    *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE NETWORK PREFIX NOT FOUND" << "\n";
                }
                continue;
            }

            price = repository->getPrice(trunkSettings->pricelist_id, networkPrefix->network_type_id);
            if (price == nullptr) {
                if (trace != nullptr) {
                    *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE PRICELIST PRICE NOT FOUND" << "\n";
                }
                continue;
            }


        } else {

            price = repository->getPrice(trunkSettings->pricelist_id, call->dst_number);
            if (price == nullptr) {
                if (trace != nullptr) {
                    *trace << "INFO|SERVICE TRUNK SETTINGS SKIPPED|CAUSE PRICELIST PRICE NOT FOUND" << "\n";
                }
                continue;
            }

        }

        if (trace != nullptr) {
            *trace << "INFO|SERVICE TRUNK SETTINGS MATCHED" << "\n";
        }

        return true;
    }

    return false;
}

void BillingCall::numberPreprocessing() {
    int order = 1;

    bool srcNumberPreprocessingDone = false;
    bool dstNumberPreprocessingDone = false;
    while (true) {
        auto numberPreprocessing = repository->getTrunkNumberPreprocessing(callInfo->trunk->id, order);
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
                *trace << "INFO|NUMBER PREPROCESSING|SET SRC_NUMBER = " << call->src_number << "\n";
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
                *trace << "INFO|NUMBER PREPROCESSING|SET DST_NUMBER = " << call->dst_number << "\n";
            }
            dstNumberPreprocessingDone = true;
        }

        order++;
    }
}

void BillingCall::processRedirectNumber() {
    if (callInfo->trunk->orig_redirect_number) {

        long long int redirect_number =  atoll(cdr->redirect_number);

        if (redirect_number > 0) {
            call->src_number = redirect_number;

            if (trace != nullptr) {
                *trace << "INFO|PROCESS REDIRECT NUMBER|SET SRC_NUMBER = " << call->src_number << "\n";
            }
        }
    }
}

void BillingCall::processGeo() {

    auto mobPrefix = repository->getMobPrefix(call->orig ? call->dst_number : call->src_number);
    if (mobPrefix != nullptr) {
        call->geo_mob = mobPrefix->mob;
        if (trace != nullptr) {
            *trace << "INFO|SET GEO_MOB = " << call->geo_mob << "\n";
        }
    }

    auto geoPrefix = repository->getGeoPrefix(call->orig ? call->dst_number : call->src_number);
    if (geoPrefix != nullptr) {
        call->geo_id = geoPrefix->geo_id;
        call->geo_operator_id = geoPrefix->geo_operator_id;;
        if (trace != nullptr) {
            *trace << "INFO|SET GEO_ID = " << call->geo_id << "\n";
        }
    }

}

void BillingCall::processDestinations() {

    auto mobPrefix = repository->getMobPrefix(call->dst_number);
    if (mobPrefix != nullptr) {
        call->mob = mobPrefix->mob;
        if (trace != nullptr) {
            *trace << "INFO|SET MOB = " << call->mob << "\n";
        }
    }

    auto geoPrefix = repository->getGeoPrefix(call->dst_number);
    if (geoPrefix != nullptr) {
        call->destination_id = getDest(geoPrefix->geo_id);
        if (trace != nullptr) {
            *trace << "INFO|SET DEST = " << call->destination_id << "\n";
        }
    }

}

int BillingCall::getDest(int geo_id) {
    callInfo->geo = repository->getGeo(geo_id);
    if (callInfo->geo == nullptr) {
        if (trace != nullptr) {
            *trace << "ERROR|GEO NOT FOUND" << "\n";
        }
        return 2;
    }

    if (!call->mob && repository->getInstanceSettings()->city_id > 0 && callInfo->geo->city_id == repository->getInstanceSettings()->city_id) {
        return -1;
    }

    auto regionIds = repository->getInstanceSettings()->getRegionIds();
    for (auto it = regionIds.begin(); it != regionIds.end(); ++it) {
        if (callInfo->geo->region_id == *it) {
            return 0;
        }
    }

    if (repository->getInstanceSettings()->country_id > 0 && callInfo->geo->country_id == repository->getInstanceSettings()->country_id) {
        return 1;
    }

    return 2;
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
            *trace << "INFO|LINE WITHOUT NUMBER|SET SRC_NUMBER = " << call->src_number << "\n";
        }
    }
}

long long int BillingCall::getNumber() {
    return call->orig ? call->src_number : call->dst_number;
}

long long int BillingCall::getRemoteNumber() {
    return call->orig ? call->dst_number : call->src_number;
}

char * BillingCall::getRoute() {
    return call->orig ? cdr->src_route : cdr->dst_route;
}

char * BillingCall::getRemoteRoute() {
    return call->orig ? cdr->dst_route : cdr->src_route;
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

void BillingCall::setupTrunk() {
    callInfo->trunk = repository->getTrunkByName(getRoute());
    if (callInfo->trunk == nullptr) {
        throw CalcException("TRUNK WAS NOT FOUND");
    }

    call->trunk_id = callInfo->trunk->id;
    call->operator_id = callInfo->trunk->code;
    call->our = callInfo->trunk->our_trunk || callInfo->trunk->auth_by_number;
}

void BillingCall::setupEffectiveServiceTrunkPricelistPrice() {
    vector<ServiceTrunk *> serviceTrunks;
    repository->getAllServiceTrunk(serviceTrunks, callInfo->trunk->id);

    for (auto serviceTrunk : serviceTrunks) {
        Pricelist * pricelist;
        PricelistPrice * price;
        if (checkServiceTrunkAvailability(serviceTrunk, call->orig ? SERVICE_TRUNK_SETTINGS_ORIGINATION : SERVICE_TRUNK_SETTINGS_TERMINATION, pricelist, price)) {
            if (callInfo->serviceTrunk == nullptr || price->price < callInfo->price->price) {
                callInfo->serviceTrunk = serviceTrunk;
                callInfo->pricelist = pricelist;
                callInfo->price = price;
            }
        }
    }
}

void BillingCall::setupServiceTrunk() {
    if (callInfo->serviceTrunk == nullptr) {
        throw CalcException("SERVICE TRUNK WAS NOT FOUND");
    }
    call->trunk_service_id = callInfo->serviceTrunk->id;
}

void BillingCall::setupServiceNumber() {
    callInfo->serviceNumber = repository->getServiceNumber(getNumber());
    if (callInfo->serviceNumber == nullptr) {
        throw CalcException("SERVICE NUMBER WAS NOT FOUND");
    }
    call->number_service_id = callInfo->serviceNumber->id;
}

void BillingCall::setupServiceTrunkForNumber() {
    callInfo->serviceTrunk = repository->getServiceTrunk(callInfo->trunk->id);

    if (!callInfo->trunk->our_trunk) {
        if (callInfo->serviceTrunk == nullptr) {
            throw CalcException("SERVICE TRUNK WAS NOT FOUND");
        }

        if (callInfo->serviceNumber->client_account_id != callInfo->serviceTrunk->client_account_id) {
            throw CalcException("SERVICE NUMBER ACCOUNT DIFFERENT FROM SERVICE TRUNK_ACCOUNT");
        }
    }

    if (callInfo->serviceTrunk != nullptr) {
        call->trunk_service_id = callInfo->serviceTrunk->id;
    }
}

void BillingCall::setupAccount() {
    if (callInfo->serviceNumber != nullptr) {
        callInfo->account = repository->getAccount(callInfo->serviceNumber->client_account_id);
    } else if (callInfo->serviceTrunk != nullptr) {
        callInfo->account = repository->getAccount(callInfo->serviceTrunk->client_account_id);
    }

    if (callInfo->account == nullptr) {
        throw CalcException("ACCOUNT WAS NOT FOUND");
    }

    callInfo->make_dt();

    call->account_id = callInfo->account->id;
}

void BillingCall::setupPricelist(int pricelist_id) {
    if (pricelist_id != 0) {
        callInfo->pricelist = repository->getPricelist(pricelist_id);
    }
    if (callInfo->pricelist == nullptr) {
        throw CalcException("PRICELIST NOT FOUND");
    }

    if (trace != nullptr) {
        *trace << "INTO|PRICELIST|";
        callInfo->pricelist->dump(*trace);
        *trace << "\n";
    }

    call->pricelist_id = callInfo->pricelist->id;
}

void BillingCall::setupPrice(long long int numberPrefix) {
    if (numberPrefix != 0) {
        callInfo->price = repository->getPrice(callInfo->pricelist->id, numberPrefix);
    }
    if (callInfo->price == nullptr) {
        throw CalcException("PRICE NOT FOUND");
    }

    if (trace != nullptr) {
        *trace << "INTO|PRICELIST|";
        callInfo->pricelist->dump(*trace);
        *trace << "\n";
    }

    call->prefix = atoll(callInfo->price->prefix);
    call->rate = callInfo->price->price;
}

void BillingCall::setupBilledTime() {
    if (callInfo->serviceNumber) {
        call->billed_time = getCallLength(
                cdr->session_time,
                callInfo->mainTariff->tariffication_by_minutes,
                callInfo->mainTariff->tariffication_full_first_minute,
                callInfo->mainTariff->tariffication_free_first_seconds
        );
    } else {
        call->billed_time = getCallLength(
                cdr->session_time,
                callInfo->pricelist->tariffication_by_minutes,
                callInfo->pricelist->tariffication_full_first_minute,
                false
        );
    }
}

void BillingCall::setupCost() {
    int paid_time = call->billed_time - call->package_time;

    if (paid_time > 0) {
        call->cost = paid_time * call->rate / 60.0;
        call->cost = call->orig ? -call->cost : call->cost;
    } else {
        call->cost = 0;
    }

    if (call->billed_time > 0) {
        call->package_credit = call->package_time * call->rate / 60.0;
        call->package_credit = call->orig ? -call->package_credit : call->package_credit;
    } else {
        call->package_credit = 0;
    }
}

void BillingCall::setupLogTariff() {
    callInfo->logTariff = repository->getTariffLog(callInfo->serviceNumber->id);
    if (callInfo->logTariff == nullptr) {
        throw CalcException("TARIFF CHANGE LOG NOT FOUND");
    }
}

void BillingCall::setupMainTariff() {
    callInfo->mainTariff = repository->getTariff(callInfo->logTariff->tariff_id_local);
    if (callInfo->mainTariff == nullptr) {
        throw CalcException("TARIFF NOT FOUND");
    }
}

void BillingCall::setupTariff() {
    int tariffId = 0;
    if (call->isLocal())
        tariffId = callInfo->logTariff->tariff_id_local;
    else if (call->isZonaMob())
        tariffId = callInfo->logTariff->tariff_id_local_mob;
    else if (call->isZonaStd())
        tariffId = callInfo->logTariff->tariff_id_russia;
    else if (call->isRussianStd())
        tariffId = callInfo->logTariff->tariff_id_russia;
    else if (call->isRussianMob())
        tariffId = callInfo->logTariff->tariff_id_russia_mob;
    else if (call->isInternational())
        tariffId = callInfo->logTariff->tariff_id_intern;

    callInfo->tariff = repository->getTariff(tariffId);
    if (callInfo->tariff == nullptr) {
        throw CalcException("TARIFF NOT FOUND");
    }
}

void BillingCall::setupPackagePricelist() {
    vector<ServicePackage *> packages;
    repository->getAllServicePackage(packages, callInfo->serviceNumber->id);

    Pricelist * pricelist;
    PricelistPrice * price;


    for(auto package : packages) {
        auto tariff = repository->getTariffPackage(package->tariff_package_id);
        if (tariff == nullptr) {
            continue;
        }

        if (tariff->pricelist_id == 0) {
            continue;
        }

        if (!matchTariffPackageDestination(tariff)) {
            continue;
        }

        pricelist = repository->getPricelist(tariff->pricelist_id);
        if (pricelist == nullptr) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE PACKAGE SKIPPED|CAUSE PRICELIST NOT FOUND" << "\n";
            }
            continue;
        }

        price = repository->getPrice(pricelist->id, call->dst_number);
        if (price == nullptr) {
            if (trace != nullptr) {
                *trace << "INFO|SERVICE PACKAGE SKIPPED|CAUSE PRICELIST PRICE NOT FOUND" << "\n";
            }
            continue;
        }

        callInfo->pricelist = pricelist;
        callInfo->price = price;
        callInfo->servicePackagePricelist = package;
        call->service_package_id = package->id;
    }
}


void BillingCall::setupFreemin() {
    if (call->billed_time == 0) {
        return;
    }

    if (!call->isLocal()) {
        return;
    }

    int tariffFreeSeconds = 60 * callInfo->mainTariff->freemin * (callInfo->mainTariff->freemin_for_number ? 1 : callInfo->serviceNumber->lines_count);
    if (trace != nullptr) {
        *trace << "INFO|FREEMIN|TARIFF " << tariffFreeSeconds << " SECONDS, TARIFF_ID: " << callInfo->mainTariff->id << "\n";
    }

    if (tariffFreeSeconds <= 0) {
        return;
    }

    StatsFreemin * stats = repository->billingData->statsFreeminGetCurrent(callInfo);

    if (trace != nullptr) {
        *trace << "INFO|FREEMIN|USED " << stats->used_seconds << " SECONDS, STAT_ID: " << stats->id << "\n";
    }

    int availableFreeSeconds = tariffFreeSeconds - stats->used_seconds;
    if (availableFreeSeconds < 0) {
        availableFreeSeconds = 0;
    }
    if (availableFreeSeconds > call->billed_time) {
        availableFreeSeconds = call->billed_time;
    }
    if (availableFreeSeconds > 0) {
        if (trace != nullptr) {
            *trace << "INFO|FREEMIN|APPLY " << availableFreeSeconds << " SECONDS, STAT_ID: " << stats->id << "\n";
        }

        call->service_package_id = 1;
        call->service_package_stats_id = stats->id;
        call->package_time = availableFreeSeconds;
    }

}

void BillingCall::setupPackagePrepaid() {
    if (call->billed_time == 0 || call->service_package_id > 0) {
        return;
    }

    vector<ServicePackage *> packages;
    repository->getAllServicePackage(packages, callInfo->serviceNumber->id);

    int packageSeconds = 0;
    ServicePackage * servicePackage = nullptr;
    TariffPackage * tariffPackage = nullptr;
    StatsPackage * statsPackage = nullptr;


    for(auto package : packages) {
        auto tariff = repository->getTariffPackage(package->tariff_package_id);
        if (tariff == nullptr) {
            continue;
        }

        if (tariff->prepaid_minutes == 0) {
            continue;
        }

        if (!matchTariffPackageDestination(tariff)) {
            continue;
        }

        StatsPackage * stats = repository->billingData->statsPackageGetCurrent(callInfo, package, tariff);

        if (trace != nullptr) {
            *trace << "DEBUG|STATS PACKAGE|";
            stats->dump(*trace);
            *trace << "\n";
        }

        int availableSeconds = stats->paid_seconds - stats->used_seconds;
        if (availableSeconds < 0) {
            availableSeconds = 0;
        }

        if (availableSeconds >= call->billed_time) {
            packageSeconds = call->billed_time;
            servicePackage = package;
            tariffPackage = tariff;
            statsPackage = stats;
            break;
        } else {
            if (availableSeconds > packageSeconds) {
                packageSeconds = availableSeconds;
                servicePackage = package;
                tariffPackage = tariff;
                statsPackage = stats;
            }
        }
    }

    if (servicePackage != nullptr) {
        call->package_time = packageSeconds;
        call->service_package_id = servicePackage->id;
        call->service_package_stats_id = statsPackage->id;
        callInfo->servicePackagePrepaid = servicePackage;
        callInfo->tariffPackagePrepaid = tariffPackage;
    }
}

bool BillingCall::matchTariffPackageDestination(TariffPackage * tariff) {
    vector<int> prefixlistIds;
    repository->getAllStatPrefixlistIdsByStatDestinationId(prefixlistIds, tariff->destination_id);

    for (int prefixlistId : prefixlistIds) {
        StatPrefixlist * prefixlist = repository->getStatPrefixlist(prefixlistId);
        if (prefixlist != nullptr) {

            if (prefixlist->type_id == STAT_PREFIXLIST_TYPE_MANUAL) {

                for (long long int prefix : prefixlist->prefixes) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|TRY MATCH NUMBER " << call->dst_number << " BY PREFIX " << prefix << "\n";
                    }

                    char tmpPrefix[20];
                    sprintf(tmpPrefix, "%lld", call->dst_number);

                    size_t len = strlen(tmpPrefix);
                    while (len > 0) {
                        tmpPrefix[len] = 0;
                        if (prefix == atoll(tmpPrefix)) {
                            if (trace != nullptr) {
                                *trace << "INFO|MATCH|OK. PREFIX " << prefix << "\n";
                            }
                            return true;
                        }
                        len -= 1;
                    }
                }

            } else if (prefixlist->type_id == STAT_PREFIXLIST_TYPE_ROSLINK) {
                if (trace != nullptr) {
                    *trace << "INFO|MATCH|TRY MATCH BY GEO\n";
                    *trace << "|STAT PREFIXLIST|";
                    prefixlist->dump(*trace);
                    *trace << "\n";
                    *trace << "|GEO|";
                    callInfo->geo->dump(*trace);
                    *trace << "\n";
                }

                if (prefixlist->sub_type == STAT_PREFIXLIST_SUBTYPE_FIXED && call->mob) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT FIXED\n";
                    }
                    continue;
                }

                if (prefixlist->sub_type == STAT_PREFIXLIST_SUBTYPE_MOBILE && !call->mob) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MOBILE\n";
                    }
                    continue;
                }

                if (prefixlist->country_id > 0 && callInfo->geo != nullptr && callInfo->geo->country_id != prefixlist->country_id) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MATCH COUNTRY\n";
                    }
                    continue;
                }

                if (prefixlist->region_id > 0 && callInfo->geo != nullptr && callInfo->geo->region_id != prefixlist->region_id) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MATCH REGION\n";
                    }
                    continue;
                }

                if (prefixlist->city_id > 0 && callInfo->geo != nullptr && callInfo->geo->city_id != prefixlist->city_id) {
                    if (trace != nullptr) {
                        *trace << "INFO|MATCH|EXIT. NOT MATCH CITY\n";
                    }
                    continue;
                }

                if (prefixlist->exclude_operators && prefixlist->operators.size() > 0) {
                    bool exclude = false;
                    for (int excludeOperatorId : prefixlist->operators) {
                        if (call->geo_operator_id == excludeOperatorId) {
                            exclude = true;
                            break;
                        }
                    }
                    if (exclude) {
                        if (trace != nullptr) {
                            *trace << "INFO|MATCH|EXIT. NOT MATCH EXCULDE OPERATORS\n";
                        }
                        continue;
                    }
                }

                if (!prefixlist->exclude_operators && prefixlist->operators.size() > 0) {
                    bool exclude = true;
                    for (int includeOperatorId : prefixlist->operators) {
                        if (call->geo_operator_id == includeOperatorId ) {
                            exclude = false;
                            break;
                        }
                    }
                    if (exclude) {
                        if (trace != nullptr) {
                            *trace << "INFO|MATCH|EXIT. NOT MATCH INCLUDE OPERATORS\n";
                        }
                        continue;
                    }
                }

                if (trace != nullptr) {
                    *trace << "INFO|MATCH|OK. BY GEO\n";
                }
                return true;
            }

        }
    }
    return false;
}