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

void logFinishedCall(const Call& call, Client* account, Repository& repository) {

    pLogMessage logCall(new LogMessage());
    logCall->type = "call";

    logCall->message = lexical_cast<string>(call.id);

    logCall->params["orig"] = call.orig ? "true" : "false";
    logCall->params["src"] = call.src_number;
    logCall->params["dst"] = call.dst_number;
    logCall->params["trunk_id"] = call.trunk_id;
    logCall->params["account_id"] = call.account_id;
    logCall->params["trunk_service_id"] = call.trunk_service_id;
    logCall->params["number_service_id"] = call.number_service_id;
    logCall->params["service_package_id"] = call.service_package_id;
    logCall->params["pricelist_id"] = call.pricelist_id;
    logCall->params["pricelist_prefix"] = lexical_cast<string>(call.prefix);
    logCall->params["geo_id"] = call.geo_id;
    logCall->params["rate"] = call.rate;
    logCall->params["cost"] = call.cost;
    logCall->params["billed_time"] = call.billed_time;
    logCall->params["package_time"] = call.package_time;

    if (account) {
	double vat_rate = repository.getVatRate(account);

	double sumBalance = repository.billingData->statsAccountGetSumBalance(account->id, vat_rate);
	double sumDay = repository.billingData->statsAccountGetSumDay(account->id, vat_rate);

	auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
	double sumBalance2 = statsAccount2->getSumBalance(account->id, vat_rate) + call.cost;
	double sumDay2 = statsAccount2->getSumDay(account->id, vat_rate) + call.cost;

	double globalBalanceSum, globalDaySum;
	fetchGlobalCounters(account->id, vat_rate, repository, & globalBalanceSum, & globalDaySum);

	double spentBalanceSum, spentDaySum;
	spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
	spentDaySum = sumDay + sumDay2 + globalDaySum;

	logCall->params["balance_stat"] = account->balance;
	logCall->params["balance_local"] = sumBalance;
	logCall->params["balance_current"] = sumBalance2;
	logCall->params["balance_global"] = globalBalanceSum;
	logCall->params["balance_realtime"] = account->balance + spentBalanceSum;
	if (account->hasCreditLimit()) {
	    logCall->params["credit_limit"] = account->credit;
	    logCall->params["credit_available"] = account->balance + account->credit + spentBalanceSum;
	}

	logCall->params["daily_local"] = sumDay;
	logCall->params["daily_current"] = sumDay2;
	logCall->params["daily_global"] = globalDaySum;
	logCall->params["daily_total"] = spentDaySum;
	if (account->hasDailyLimit()) {
	    logCall->params["daily_limit"] = account->limit_d;
	    logCall->params["daily_available"] = account->limit_d + spentDaySum;
	}
    }    

    Log::info(logCall);

}

void logFinishedCall(const Call& origCall, const Call& termCall, Client* origAccount, Client* termAccount, Trunk* origTrunk, Trunk* termTrunk, Repository& repository) {

    // TODO: адекватное логирование звонков

    // interconnect_cost ??? vat ??? как влияют на стоимость и себестоимость?

    // Исходящий - если origCall.our && (!termCall.our || termCall.our && termTrunk && termTrunk->route_table_id)
    // Входящий  - если termCall.our && (!origCall.our || origCall.our && origTrunk && origTrunk->route_table_id)
    // Сразу два звонка - если origCall.our && termCall.our && origTrunk && origTrunk->route_table_id && termTrunk && termTrunk->route_table_id
    // Исходящий стоимость: origCall.cost, себестоимость: termCall.cost
    // Входящий стоимость: termCall.cost, себестоимость: origCall.cost ; знак имеет значение
    // Сразу два звонка:
    //  Исходящий стоимость: origCall.cost, себестоимость: 0
    //  Входящий стоимость: termCall.cost, себестоимость: 0

    logFinishedCall(origCall, origAccount, repository);
    logFinishedCall(termCall, termAccount, repository);
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
