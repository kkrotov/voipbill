#include "ThreadLimitControl.h"
#include "../classes/AppBill.h"
#include "../classes/UdpControlClient.h"
#include "../common.h"

ThreadLimitControl::ThreadLimitControl() {
    id = idName();
    name = "Limit control";
}

bool ThreadLimitControl::ready() {
    if (!repository.billingData->ready()) {
        return false;
    }

    if (!repository.currentCalls->ready()) {
        return false;
    }


    return true;
}

void ThreadLimitControl::run() {


    if (!repository.prepare()) {
        return;
    }

    auto calls = repository.currentCalls->getCallsWaitingSaving();
    for (auto &call : *calls.get()) {

        pLogMessage logRequest(new LogMessage());

        if (limitControlKillNeeded(call, logRequest)) {
            logRequest->type = "kill";
            logRequest->params["orig"] = call.orig ? "true" : "false";
            logRequest->params["src"] = call.src_number;
            logRequest->params["dst"] = call.dst_number;
            logRequest->params["trunk_id"] = call.trunk_id;
            logRequest->params["account_id"] = call.account_id;
            logRequest->params["trunk_service_id"] = call.trunk_service_id;
            logRequest->params["number_service_id"] = call.number_service_id;
            logRequest->params["service_package_id"] = call.service_package_id;
            logRequest->params["pricelist_id"] = call.pricelist_id;
            logRequest->params["pricelist_prefix"] = lexical_cast<string>(call.prefix);
            logRequest->params["geo_id"] = call.geo_id;
            logRequest->params["rate"] = call.rate;
            logRequest->params["cost"] = call.cost;

            Log::notice(logRequest);

            string phone = lexical_cast<string>(call.src_number);
            string call_id = lexical_cast<string>(call.cdr_call_id);
            UdpControlClient::kill(phone, call_id);

        }

    }

}

bool ThreadLimitControl::limitControlKillNeeded(Call &call, pLogMessage &logRequest) {

    bool auto_lock_finance = true;

    if(repository.prepare()) {
        InstanceSettings *instanceSettings = repository.getInstanceSettings(app().conf.instance_id);
        auto_lock_finance = (instanceSettings!= nullptr)? instanceSettings->auto_lock_finance:false;
    }


    if (call.is_service_number) {
        return false;
    }

    if (call.account_id == 0) {
        return false;
    }

    auto client = repository.getAccount(call.account_id);
    if (client == nullptr) {
        Log::error("Account #" + lexical_cast<string>(call.account_id) + " not found");
        return false;
    }

    double vat_rate = repository.getVatRate(client);

    double sumBalance = repository.billingData->statsAccountGetSumBalance(call.account_id, vat_rate);
    double sumDay = repository.billingData->statsAccountGetSumDay(call.account_id, vat_rate);
    double sumMNDay = repository.billingData->statsAccountGetSumMNDay(call.account_id, vat_rate);

    auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
    double sumBalance2 = statsAccount2->getSumBalance(call.account_id, vat_rate);
    double sumDay2 = statsAccount2->getSumDay(call.account_id, vat_rate);
    double sumMNDay2 = statsAccount2->getSumMNDay(call.account_id, vat_rate);

    double globalBalanceSum, globalDaySum, globalDayMNSum;
    fetchGlobalCounters(call.account_id, globalBalanceSum, globalDaySum, globalDayMNSum, vat_rate);

    double spentBalanceSum, spentDaySum, spentDayMNSum;
    spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
    spentDaySum = sumDay + sumDay2 + globalDaySum;
    spentDayMNSum = sumMNDay + sumMNDay2 + globalDayMNSum;

    logRequest->params["balance_stat"] = client->balance;
    logRequest->params["balance_local"] = sumBalance;
    logRequest->params["balance_current"] = sumBalance2;
    logRequest->params["balance_global"] = globalBalanceSum;
    logRequest->params["balance_realtime"] = client->balance + spentBalanceSum;
    if (client->hasCreditLimit()) {
        logRequest->params["credit_limit"] = client->credit;
        logRequest->params["credit_available"] = client->balance + client->credit + sumBalance + sumBalance2 + globalBalanceSum;
    }

    logRequest->params["daily_local"] = sumDay;
    logRequest->params["daily_current"] = sumDay2;
    logRequest->params["daily_global"] = globalDaySum;
    logRequest->params["daily_total"] = spentDaySum;
    logRequest->params["daily_mn_local"] = sumMNDay;
    logRequest->params["daily_mn_current"] = sumMNDay2;
    logRequest->params["daily_mn_global"] = globalDayMNSum;
    logRequest->params["daily_mn_total"] = spentDayMNSum;
    if (client->hasDailyLimit()) {
        logRequest->params["daily_limit"] = client->limit_d;
        logRequest->params["daily_available"] = client->limit_d + spentDaySum;
    }
    if (client->hasDailyMNLimit()) {
        logRequest->params["daily_mn_limit"] = client->limit_d_mn;
        logRequest->params["daily_mn_available"] = client->limit_d_mn + spentDayMNSum;
    }
    if (client->is_blocked) {
        logRequest->params["block_full_flag"] = "true";
    }

    if (client->disabled) {
        logRequest->params["block_mgmn_flag"] = "true";
    }

    if (client->isConsumedCreditLimit(spentBalanceSum)) {
        logRequest->params["block_credit_flag"] = "true";
    }

    if (client->isConsumedDailyLimit(spentDaySum)) {
        logRequest->params["block_daily_flag"] = "true";
    }

    // Глобальная блокировка
    if (client->is_blocked) {
        logRequest->params["kill_reason"] = "full_block";

        logRequest->message =
                "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Block Global<br/>\n";

        return true;
    }

    if (call.trunk_service_id != 0) {

        if (client->isConsumedCreditLimit(spentBalanceSum) && auto_lock_finance) {
            logRequest->params["kill_reason"] = "credit_limit";

            logRequest->message =
                    "KILL: trunk #" + lexical_cast<string>(call.trunk_service_id) + ": Client Id: " + lexical_cast<string>(client->id) + ", Credit limit: " + string_fmt("%.2f", client->balance + client->credit + sumBalance + sumBalance2 + globalBalanceSum) + " = " +
                    string_fmt("%.2f", client->balance) + " (balance) + " + string_fmt("%d", client->credit) + " (credit) + " + string_fmt("%.2f", sumBalance) + " (local) + " + string_fmt("%.2f", sumBalance2) + " (current) + " + string_fmt("%.2f", globalBalanceSum) + " (global) <br/>\n";

            return true;

        }

    } else if (call.number_service_id != 0 && call.orig) {

        // Блокировка МГМН
        if (!call.isLocal() && client->disabled) {
            logRequest->params["kill_reason"] = "mgmn_block";

            logRequest->message =
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Block MGMN<br/>\n";
            return true;
        }

        // Блокировка МГМН если превышен лимит кредита
        if (!call.isLocal()  && client->isConsumedCreditLimit(spentBalanceSum) && auto_lock_finance) {
            logRequest->params["kill_reason"] = "credit_limit";

            logRequest->message =
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Credit limit: " + string_fmt("%.2f", client->balance + client->credit + sumBalance + sumBalance2 + globalBalanceSum) + " = " +
                    string_fmt("%.2f", client->balance) + " (balance) + " + string_fmt("%d", client->credit) + " (credit) + " + string_fmt("%.2f", sumBalance) + " (local) + " + string_fmt("%.2f", sumBalance2) + " (current) + " + string_fmt("%.2f", globalBalanceSum) + " (global) <br/>\n";

            return true;
        }

        // Блокировка МН
        if (call.isInternational() && client->isConsumedDailyMNLimit(spentDayMNSum)) {
            logRequest->params["kill_reason"] = "mn_block";

            logRequest->message =
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " +
                    lexical_cast<string>(call.dst_number) + " : Block MN overrun<br/>\n";
            return true;
        }

        // Блокировка МГМН если превышен дневной лимит
        if (!call.isLocal() && client->isConsumedDailyLimit(spentDaySum)) {
            logRequest->params["kill_reason"] = "daily_limit";

            logRequest->message =
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Daily limit: " + string_fmt("%.2f", client->limit_d + sumDay + sumDay2 + globalDaySum) + " = " +
                    string_fmt("%d", client->limit_d) + " (limit_d) + " + string_fmt("%.2f", sumDay) + " (local) + " + string_fmt("%.2f", sumDay2) + " (current) + " + string_fmt("%.2f", globalDaySum) + " (global) <br/>\n";

            return true;
        }

    }

    return false;

}

void ThreadLimitControl::fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum,
                                             double &globalDayMNSum, double vat_rate) {

    GlobalCounters * globalCounter = nullptr;
    if (repository.data->globalCounters.ready()) {
        globalCounter = repository.data->globalCounters.get()->find(accountId);
    }

    if (globalCounter != nullptr) {
        globalBalanceSum = globalCounter->sumBalance(vat_rate);
        globalDaySum = globalCounter->sumDay(vat_rate);
        globalDayMNSum = globalCounter->sumMNDay(vat_rate);
    } else {
        globalBalanceSum = 0.0;
        globalDaySum = 0.0;
        globalDayMNSum = 0.0;
    }
}

