#include "Billing.h"
#include "BillingCall.h"

void Billing::setData(DataContainer *data) {
    repository.data = data;
}

void Billing::setBillingData(DataBillingContainer *billingData) {
    repository.billingData = billingData;
}

void Billing::calcCurrentCalls() {

    if (!repository.currentCalls->ready()) {
        return;
    }

    BillingCall billingCall(&repository);

    auto currentCdrs = repository.currentCalls->currentCdr.get();

    shared_ptr<vector<Call>> callsWaitSaving(new vector<Call>());
    shared_ptr<StatsAccountManager> statsAccount(new StatsAccountManager());
    shared_ptr<StatsFreeminManager> statsFreemin(new StatsFreeminManager());
    shared_ptr<StatsPackageManager> statsPackage(new StatsPackageManager());

    for (size_t i = 0; i < currentCdrs->size(); i++) {
        auto cdr = currentCdrs->get(i);

        if (!repository.prepare(cdr->connect_time)) {
            break;
        }

        Call origCall = Call(cdr, CALL_ORIG);
        Call termCall = Call(cdr, CALL_TERM);

        CallInfo origCallInfo;
        CallInfo termCallInfo;

        billingCall.calc(&origCall, &origCallInfo, cdr);

        termCall.src_number = origCall.src_number;
        termCall.dst_number = origCall.dst_number;
        billingCall.calc(&termCall, &termCallInfo, cdr);

        statsAccount.get()->add(&origCallInfo);
        statsFreemin.get()->add(&origCallInfo);
        statsPackage.get()->add(&origCallInfo);
        callsWaitSaving->push_back(origCall);

        statsAccount.get()->add(&termCallInfo);
        statsFreemin.get()->add(&termCallInfo);
        statsPackage.get()->add(&termCallInfo);
        callsWaitSaving->push_back(termCall);
    }

    repository.currentCalls->setCallsWaitingSaving(callsWaitSaving);
    repository.currentCalls->setStatsAccount(statsAccount);
    repository.currentCalls->setStatsFreemin(statsFreemin);
    repository.currentCalls->setStatsPackage(statsPackage);
}

void fetchGlobalCounters(int accountId, double vat_rate, Repository& repository, double* globalBalanceSum, double* globalDaySum) {

    GlobalCounters * globalCounter = nullptr;
    if (repository.data->globalCounters.ready()) {
        globalCounter = repository.data->globalCounters.get()->find(accountId);
    }

    if (globalCounter) {
        * globalBalanceSum = globalCounter->sumBalance(vat_rate);
        * globalDaySum = globalCounter->sumDay(vat_rate);
    } else {
        * globalBalanceSum = 0.0;
        * globalDaySum = 0.0;
    }
}


void logFinishedCall(const Call& origCall, const Call& termCall, Client* origAccount, Client* termAccount, Trunk* origTrunk, Trunk* termTrunk, Repository& repository) {

    // interconnect_cost ??? vat ??? как влияют на стоимость и себестоимость?

    // TODO? Можно применить классификатор для определения типа звонков для упрощения аналитики:
    //
    // Исходящий - если origCall.our && (!termCall.our || termCall.our && termTrunk && termTrunk->route_table_id)
    // Входящий  - если termCall.our && (!origCall.our || origCall.our && origTrunk && origTrunk->route_table_id)
    // Сразу два звонка - если origCall.our && termCall.our && origTrunk && origTrunk->route_table_id && termTrunk && termTrunk->route_table_id
    // Исходящий стоимость: origCall.cost, себестоимость: termCall.cost
    // Входящий стоимость: termCall.cost, себестоимость: origCall.cost ; знак имеет значение
    // Сразу два звонка:
    //  Исходящий стоимость: origCall.cost, себестоимость: 0
    //  Входящий стоимость: termCall.cost, себестоимость: 0
    // Также у исходящих может быть нулевая стоимость терминации, когда звонок идёт через наш транк в другом регионе,
    // и нулевая стоимость оригинации - для входящей части этого же звонка в другом регионе.
    // К сожалению, merge'ить такие звонки мы не можем. И маржу по ним считать некорректно.
    // Продолжительность звонка
    // Продолжительность для нашего клиента
    // Продолжительность для нас
    // Пакет и расход пакета.
    // ID CDR'а
    // Признак звонка на мобильный/с мобильного.

    pLogMessage logCall(new LogMessage());
    logCall->type = "call";

    logCall->message = lexical_cast<string>(origCall.cdr_id);

    logCall->params["orig_id"] = origCall.id;
    logCall->params["term_id"] = termCall.id;

    logCall->params["src"] = origCall.src_number;
    logCall->params["dst"] = origCall.dst_number;

    logCall->params["orig_our"] = origCall.our ? "client" : "operator";
    logCall->params["term_our"] = termCall.our ? "client" : "operator";

    logCall->params["orig_trunk_id"] = origCall.trunk_id;
    logCall->params["term_trunk_id"] = termCall.trunk_id;

    logCall->params["orig_account_id"] = origCall.account_id;
    logCall->params["term_account_id"] = termCall.account_id;

    if (origCall.trunk_service_id) {
        logCall->params["orig_trunk_service_id"] = origCall.trunk_service_id;
    }

    if (termCall.trunk_service_id) {
        logCall->params["term_trunk_service_id"] = termCall.trunk_service_id;
    }

    if (origCall.number_service_id) {
        logCall->params["orig_number_service_id"] = origCall.number_service_id;
    }

    if (termCall.number_service_id) {
        logCall->params["term_number_service_id"] = termCall.number_service_id;
    }

    logCall->params["orig_service_package_id"] = origCall.service_package_id;
    logCall->params["term_service_package_id"] = termCall.service_package_id;

    logCall->params["orig_pricelist_id"] = origCall.pricelist_id;
    logCall->params["orig_pricelist_prefix"] = lexical_cast<string>(origCall.prefix);

    logCall->params["term_pricelist_id"] = termCall.pricelist_id;
    logCall->params["term_pricelist_prefix"] = lexical_cast<string>(termCall.prefix);

    logCall->params["geo_id"] = origCall.geo_id;

    logCall->params["orig_rate"] = origCall.rate;
    logCall->params["orig_cost"] = -origCall.cost;

    logCall->params["term_rate"] = termCall.rate;
    logCall->params["term_cost"] = termCall.cost;

    if (abs(origCall.cost) > 0.000001 || abs(termCall.cost) > 0.000001) {
        auto grossMargin = -origCall.cost - termCall.cost;
        if (grossMargin > 0.000001) {
            logCall->params["gross_margin"] = grossMargin;
            logCall->params["gross_margin_percent"] = grossMargin / -origCall.rate * 100.0;
        } else {
            logCall->params["gross_margin_negative"] = -grossMargin;
            logCall->params["gross_margin_negative_percent"] = grossMargin / origCall.rate * 100.0;
        }
    }

    logCall->params["orig_billed_time"] = origCall.billed_time;
    logCall->params["orig_package_time"] = origCall.package_time;

    logCall->params["term_billed_time"] = termCall.billed_time;
    logCall->params["term_package_time"] = termCall.package_time;

    if (origAccount) {
	double vat_rate = repository.getVatRate(origAccount);

	double sumBalance = repository.billingData->statsAccountGetSumBalance(origAccount->id, vat_rate);
	double sumDay = repository.billingData->statsAccountGetSumDay(origAccount->id, vat_rate);

	auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
	double sumBalance2 = statsAccount2->getSumBalance(origAccount->id, vat_rate) + origCall.cost;
	double sumDay2 = statsAccount2->getSumDay(origAccount->id, vat_rate) + origCall.cost;

	double globalBalanceSum, globalDaySum;
	fetchGlobalCounters(origAccount->id, vat_rate, repository, & globalBalanceSum, & globalDaySum);

	double spentBalanceSum, spentDaySum;
	spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
	spentDaySum = sumDay + sumDay2 + globalDaySum;

	logCall->params["orig_balance_stat"] = origAccount->balance;
	logCall->params["orig_balance_local"] = sumBalance;
	logCall->params["orig_balance_current"] = sumBalance2;
	logCall->params["orig_balance_global"] = globalBalanceSum;
	logCall->params["orig_balance_realtime"] = origAccount->balance + spentBalanceSum;
	if (origAccount->hasCreditLimit()) {
	    logCall->params["orig_credit_limit"] = origAccount->credit;
	    logCall->params["orig_credit_available"] = origAccount->balance + origAccount->credit + spentBalanceSum;
	}

	logCall->params["orig_daily_local"] = sumDay;
	logCall->params["orig_daily_current"] = sumDay2;
	logCall->params["orig_daily_global"] = globalDaySum;
	logCall->params["orig_daily_total"] = spentDaySum;
	if (origAccount->hasDailyLimit()) {
	    logCall->params["orig_daily_limit"] = origAccount->limit_d;
	    logCall->params["orig_daily_available"] = origAccount->limit_d + spentDaySum;
	}
    } 
   
    if (termAccount) {
	double vat_rate = repository.getVatRate(termAccount);

	double sumBalance = repository.billingData->statsAccountGetSumBalance(termAccount->id, vat_rate);
	double sumDay = repository.billingData->statsAccountGetSumDay(termAccount->id, vat_rate);

	auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
	double sumBalance2 = statsAccount2->getSumBalance(termAccount->id, vat_rate) + termCall.cost;
	double sumDay2 = statsAccount2->getSumDay(termAccount->id, vat_rate) + termCall.cost;

	double globalBalanceSum, globalDaySum;
	fetchGlobalCounters(termAccount->id, vat_rate, repository, & globalBalanceSum, & globalDaySum);

	double spentBalanceSum, spentDaySum;
	spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
	spentDaySum = sumDay + sumDay2 + globalDaySum;

	logCall->params["term_balance_stat"] = termAccount->balance;
	logCall->params["term_balance_local"] = sumBalance;
	logCall->params["term_balance_current"] = sumBalance2;
	logCall->params["term_balance_global"] = globalBalanceSum;
	logCall->params["term_balance_realtime"] = termAccount->balance + spentBalanceSum;
	if (termAccount->hasCreditLimit()) {
	    logCall->params["term_credit_limit"] = termAccount->credit;
	    logCall->params["term_credit_available"] = termAccount->balance + termAccount->credit + spentBalanceSum;
	}

	logCall->params["term_daily_local"] = sumDay;
	logCall->params["term_daily_current"] = sumDay2;
	logCall->params["term_daily_global"] = globalDaySum;
	logCall->params["term_daily_total"] = spentDaySum;
	if (termAccount->hasDailyLimit()) {
	    logCall->params["term_daily_limit"] = termAccount->limit_d;
	    logCall->params["term_daily_available"] = termAccount->limit_d + spentDaySum;
	}
    }    

    Log::info(logCall);
}


void Billing::calc(bool realtimePurpose) {

    if (!repository.billingData->ready()) {
        return;
    }

    BillingCall billingCall(&repository);


    while (true) {

        if (repository.billingData->callsQueueSize() >= CALLS_MAX_QUEUE_SIZE) {
            break;
        }

        Cdr *cdr = repository.billingData->getFirstCdr();
        if (cdr == nullptr) {
            break;
        }

        if (!repository.prepare(cdr->connect_time)) {
            break;
        }

        long long int lastCallId = repository.billingData->getCallsLastId();

        Call origCall = Call(cdr, CALL_ORIG);
        Call termCall = Call(cdr, CALL_TERM);

        CallInfo origCallInfo;
        CallInfo termCallInfo;

        origCall.id = lastCallId + 1;
        origCall.peer_id = lastCallId + 2;
        termCall.id = lastCallId + 2;
        termCall.peer_id = lastCallId + 1;

        billingCall.calc(&origCall, &origCallInfo, cdr);

        termCall.src_number = origCall.src_number;
        termCall.dst_number = origCall.dst_number;
        billingCall.calc(&termCall, &termCallInfo, cdr);

        repository.billingData->addCall(&origCallInfo);
        repository.billingData->addCall(&termCallInfo);

        repository.billingData->removeFirstCdr();

        if (realtimePurpose) {
            // Логируем завершённый звонок
            logFinishedCall(origCall, termCall, origCallInfo.account, termCallInfo.account,
                            origCallInfo.trunk, termCallInfo.trunk, repository);
        }
    }

}
