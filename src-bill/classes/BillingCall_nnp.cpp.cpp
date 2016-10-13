#include "BillingCall.h"
#include "../classes/CalcException.h"

void BillingCall::calcOrigNNPByNumber() {

    set<int> nnpDestinationIds; // Вычисляем все nnp-направления для номера получателя
    vector<NNPAccountTariffLight> nnpAccountTariffLightList;
    vector<ServiceNumber> serviceNumber;

    // Загружаем список номеров у на лицевом счете.
    repository->getServiceNumberByClientID(serviceNumber, call->account_id);

    if (callInfo->serviceNumber == nullptr) throw CalcException("NOT FOUND serviceNumber");

    // Загружаем список nnp-тарифов на лицевом счете действующих на время соединения
    repository->getActiveNNPAccountTariffLight(nnpAccountTariffLightList, call->account_id,
                                               call->connect_time, callInfo->serviceNumber->id);

    if (nnpAccountTariffLightList.size() == 0) throw CalcException("NOT FOUND AccountTariffLight");

    // Вычисляем все nnp-направления для B-номера звонка
    NNPNumberRange *nnpNumberRange = repository->getNNPNumberRange(call->dst_number, trace);

    if (nnpNumberRange == nullptr) throw CalcException("NOT FOUND NumberRange");

    repository->getNNPDestinationByNumberRange(nnpDestinationIds, nnpNumberRange, trace);

    if (nnpDestinationIds.size() == 0) throw CalcException("NOT FOUND Destination");

    setupBilledTimeNNP(*nnpAccountTariffLightList.begin());

    auto effectivePackagePrice = setupNNPPackagePrice(nnpAccountTariffLightList,
                                                      nnpDestinationIds);      // Подбираем самый выгодный пакет с ценой для звонка.
    auto effectivePackagePricelist = setupNNPPackagePricelist(nnpAccountTariffLightList,
                                                              nnpDestinationIds);  // Подбираем самый выгодный пакет с прайслистом для звонка.

    auto effectivePackageMinute = setupNNPPackageMinute(nnpAccountTariffLightList,
                                                        nnpDestinationIds); // Ищем неизрасходованный пакет с минутами для этого направления

    // Проверяем на минимальность цены у пакета с ценой и пакета с прайс-листом.
    if (effectivePackagePricelist.second != 0 && effectivePackagePrice.second != 0) {
        if (fabs(effectivePackagePrice.first) <= fabs(effectivePackagePricelist.first)) {
            call->rate = effectivePackagePrice.first;
            call->nnp_package_price_id = effectivePackagePrice.second;
            if (trace != nullptr) {
                *trace << "INFO|NNP|APPLY NNP_PackagePrice id=" << call->nnp_package_price_id <<
                ", rate: " << call->rate << "\n";
            }
        } else {
            call->rate = effectivePackagePricelist.first;
            call->nnp_package_pricelist_id = effectivePackagePricelist.second;
            if (trace != nullptr) {
                *trace << "INFO|NNP|APPLY NNP_PackagePricelist id=" << call->nnp_package_pricelist_id <<
                ", rate: " << call->rate << "\n";
            }
        }
    } else {
        if (effectivePackagePricelist.second == 0 && effectivePackagePrice.second != 0) {
            call->rate = effectivePackagePrice.first;
            call->nnp_package_price_id = effectivePackagePrice.second;
            if (trace != nullptr) {
                *trace << "INFO|NNP|APPLY NNP_PackagePrice id=" << call->nnp_package_price_id <<
                ", rate: " << call->rate << "\n";
            }
        }
        if (effectivePackagePrice.second == 0 && effectivePackagePricelist.second != 0) {
            call->rate = effectivePackagePricelist.first;
            call->nnp_package_pricelist_id = effectivePackagePricelist.second;
            if (trace != nullptr) {
                *trace << "INFO|NNP|APPLY NNP_PackagePricelist id=" << call->nnp_package_pricelist_id <<
                ", rate: " << call->rate << "\n";
            }
        }
        if (effectivePackagePrice.second == 0 && effectivePackagePricelist.second == 0) {
            call->rate = 0;
        }
    }

    call->package_time = 0;

    if (effectivePackageMinute.second != 0) {
        // пакет с неизрасходованными минутами есть, применяем его.
        if (trace != nullptr) {
            *trace << "DETAIL|NNP|SOLVED BILLED_TIME = " << call->billed_time << "\n";
        }
        int avalibleNNPPackageSeconds = effectivePackageMinute.first;

        if (avalibleNNPPackageSeconds < 0) {
            avalibleNNPPackageSeconds = 0;
        }
        if (avalibleNNPPackageSeconds > call->billed_time) {
            avalibleNNPPackageSeconds = call->billed_time;
        }
        if (avalibleNNPPackageSeconds > 0) {
            if (trace != nullptr) {
                *trace << "DETAIL|NNP|SOLVED avalibleNNPPackageSeconds = " << avalibleNNPPackageSeconds << "\n";
            }

            call->nnp_package_minute_id = effectivePackageMinute.second; //  Это значит, что минуты выбираются из предоплаченных локального тарифа
            call->package_time = avalibleNNPPackageSeconds;

            NNPPackageMinute *nnpPackageMinute = repository->getNNPPackageMinute(call->nnp_package_minute_id, trace);

            for (auto itNnpAccountTariffLight :nnpAccountTariffLightList) {
                if (itNnpAccountTariffLight.nnp_tariff_id == nnpPackageMinute->nnp_tariff_id) {
                    StatsNNPPackageMinute *stats = repository->billingData->statsNNPPackageMinuteGetCurrent(
                            cdr->connect_time,
                            callInfo->account, nnpPackageMinute, &itNnpAccountTariffLight);


                    call->stats_nnp_package_minute_id = stats->id; // запоминаем применный к вызову пакет из billing.stats_packege
                    if (trace != nullptr) {
                        *trace << "INFO|NNP|APPLY NNP PackageMimute id=" << call->nnp_package_minute_id
                        << ", " << avalibleNNPPackageSeconds << " SECONDS, StatsNNPPackageMinute: " << stats->id <<
                        ", Remain seconds: " << effectivePackageMinute.first << "\n";
                    }
                    break;
                }
            }


        }
    }

    setupNNPCost();
}


/********************************************************************************************************************
 *  Рачсчитываем nnp- параметры для пдеча вызова.
 */


void BillingCall::processNNP() {
    NNPNumberRange *nnpNumberRange;

    if (call->orig) nnpNumberRange = repository->getNNPNumberRange(call->src_number, trace);
    else nnpNumberRange = repository->getNNPNumberRange(call->dst_number, trace);

    if (nnpNumberRange != nullptr) {
        this->callInfo->nnpNumberRange = nnpNumberRange;
        this->call->nnp_operator_id = nnpNumberRange->nnp_operator_id;
        this->call->nnp_region_id = nnpNumberRange->nnp_region_id;
        this->call->nnp_city_id = nnpNumberRange->nnp_city_id;
        this->call->nnp_country_prefix = nnpNumberRange->country_prefix;
        this->call->nnp_ndc = nnpNumberRange->ndc;
        this->call->nnp_is_mob = nnpNumberRange->is_mob;
        if (trace != nullptr) {
            *trace << "INFO|SET NNP_NUMBER_RANGE|";
            callInfo->nnpNumberRange->dump(*trace);
            *trace << "\n";
            *trace << "INFO|SET NNP_OPERATOR_ID = " << call->nnp_operator_id << "\n";
            *trace << "INFO|SET NNP_REGION_ID = " << call->nnp_region_id << "\n";
            *trace << "INFO|SET NNP_CITY_ID = " << call->nnp_city_id << "\n";
            *trace << "INFO|SET NNP_COUNTRY_PREFIX = " << call->nnp_country_prefix << "\n";
            *trace << "INFO|SET NNP_NDC = " << call->nnp_ndc << "\n";
            *trace << "INFO|SET NNP_NIS_MOB = " << call->nnp_is_mob << "\n";
        }
    }
}

/******************************************************************************************************************
 *   вычисляется тарифицируемая длительность звонка согласно настройкам в действующем account_tariff_light
 *   (бесплатные секунды, тарификация по мин/по сек) действующего тарифа для расчитываемого плеча.
 */

void BillingCall::setupBilledTimeNNP(NNPAccountTariffLight nnpAccountTariffLight) {
    call->billed_time = getCallLength(
            cdr->session_time,
            nnpAccountTariffLight.tariffication_by_minutes,
            nnpAccountTariffLight.tariffication_full_first_minute,
            nnpAccountTariffLight.tariffication_free_first_seconds
    );
}

/******************************************************************************************************************
 *   Учитываем, сколько секунд из тарифицированного вызова можно учесть из подключенных nnp-пакетов
 *   Поиск подходящего действующего nnp-пакета с неизрасходованными минутами
 *   для ранее расчитанного nnp-направления звонка
 *   Возвращает пару - максимальный отстаток минут в ННП- пакете, ключ ННП- пакета с минутами
 *
 *   Внимание: Потенциально опасная ситуация - когда длительность звонка больше чем остаток минут в каждом из оставшихся
 *   ннп- пакетов с минутами. получится так, что примениться максимально полный пакет, а остаток минут тарифицируется за деньги
 */

pair<int, int> BillingCall::setupNNPPackageMinute(vector<NNPAccountTariffLight> &nnpAccountTariffLightList,
                                                  set<int> &nnpDestinationIds) {

    vector<NNPPackageMinute> nnpPackageMinuteList;
    set<pair<int, int >> EffectiveNnpPackageMinuteList;
    map<int, int> tariffIdAccountTariffLightId;

    // Загружаем все nnp-пакеты с минутами для тарифов этого лицевого счета
    for (auto it1 = nnpAccountTariffLightList.begin(); it1 != nnpAccountTariffLightList.end(); it1++) {
        repository->getNNPPackageMinuteByTariff(nnpPackageMinuteList, it1->nnp_tariff_id, it1->coefficient);
        tariffIdAccountTariffLightId[it1->nnp_tariff_id] = it1->id;
    }

    // Оставляем только те пакеты с минутами, в которых есть nnp-направления номера А , учитываем остаток в минутах
    for (auto it2 = nnpPackageMinuteList.begin(); it2 != nnpPackageMinuteList.end(); it2++) {
        if (nnpDestinationIds.count(it2->nnp_destination_id) > 0) {
            int used_seconds = repository->billingData->statsNNPPackageMinuteGetUsedSeconds(call->account_id,
                                                                                            it2->id, cdr->connect_time);
            int global_used_seconds = repository->data->globalNNPPackageMinuteCounters.
                    get()->getGlobalCounter(tariffIdAccountTariffLightId[it2->nnp_tariff_id], it2->id);


            if (it2->minute * 60 - used_seconds - global_used_seconds > 0)
                EffectiveNnpPackageMinuteList.insert(
                        pair<int, int>((it2->minute * 60 - used_seconds - global_used_seconds), it2->id));
        }
    }

    if (EffectiveNnpPackageMinuteList.size() > 0)
        return *EffectiveNnpPackageMinuteList.rbegin();
    else
        return pair<int, int>(0, 0);

}

/******************************************************************************************************************
 *   Для всех действующих на лицевом счете тарифов выбираем пакет с ценой с минимальной ценой для направления вызова
 *   Возвращает пару - минимальная цена , ключ ННП- пакета с ценой
 */


pair<double, int> BillingCall::setupNNPPackagePrice(vector<NNPAccountTariffLight> &nnpAccountTariffLightList,
                                                    set<int> &nnpDestinationIds)      // Подбираем самый выгодный пакет с ценой для звонка.
{
    set<pair<double, int>> resultNNPPackagePriceIds;

    for (auto nnpAccountTariffLight:nnpAccountTariffLightList) {
        repository->findNNPPackagePriceIds(resultNNPPackagePriceIds, nnpAccountTariffLight.nnp_tariff_id,
                                           nnpDestinationIds,
                                           trace);
    }

    if (resultNNPPackagePriceIds.size() == 0) return pair<double, int>(0, 0);
    else return *(resultNNPPackagePriceIds.begin());

}

/**************************************************************************************************************************
 *   Для всех действующих на лицевом счете тарифов выбираем пакет с прайслистом с минимальной ценой для направления вызова
 *   Возвращает пару - минимальная цена , ключ ННП- пакета с прайслистом
 */

pair<double, int> BillingCall::setupNNPPackagePricelist(
        vector<NNPAccountTariffLight> &nnpAccountTariffLightList,
        set<int> &nnpDestinationIds)  // Подбираем самый выгодный пакет с прайслистом для звонка.
{
    set<pair<double, int>> resultNNPPackagePricelistIds;
    for (auto nnpAccountTariffLight:nnpAccountTariffLightList) {
        repository->findNNPPackagePricelistIds(resultNNPPackagePricelistIds, nnpAccountTariffLight.nnp_tariff_id,
                                               call->dst_number, trace);
    }
    if (resultNNPPackagePricelistIds.size() == 0) return pair<double, int>(0, 0);
    else return *(resultNNPPackagePricelistIds.begin());

}

/****************************************************************************************************************
 *   Расчет стоимости плеча. С учетом остатка по найденому пакету
 *   и знака (зарабатывает или тратит клиент на этом плече)
 *   */

void BillingCall::setupNNPCost() {
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
