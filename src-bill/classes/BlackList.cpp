#include "BlackList.h"

#include "UdpControlClient.h"

bool BlackList::fetch() {
    vector<string> curr_list;
    if (!udp_blacklist(curr_list)) {
        //Log::error("Cant fetch black list from opanca");
        return false;
    }

    lock_guard<Spinlock> guard(lock);

    {
        blacklist.clear();
        for(string &phone : curr_list) {
            blacklist.insert(phone);
        }
    }

    return true;
}

void BlackList::push(set<string> &wanted_blacklist) {

    vector<string> list;

    {
        lock_guard<Spinlock> guard(lock);

        list_to_add.clear();
        list_to_del.clear();

        for (auto phone : blacklist) {
            auto it = wanted_blacklist.find(phone);
            if (it == wanted_blacklist.end()) {
                list_to_del.insert(phone);
            }
        }

        for (auto phone : wanted_blacklist) {
            auto it = blacklist.find(phone);
            if (it == blacklist.end()) {
                list_to_add.insert(phone);
            }
        }
    }

    for (auto phone : list_to_add) {
        if (!udp_lock(phone)) {
            continue;
        }

        {
            lock_guard<Spinlock> guard(lock);
            blacklist.insert(phone);
        }

        log_lock_phone(phone);
    }

    for (auto phone : list_to_del) {
        if (!udp_unlock(phone)) {
            continue;
        }

        {
            lock_guard<Spinlock> guard(lock);
            blacklist.erase(phone);
        }

        log_unlock_phone(phone);
    }
}

void BlackList::log_lock_phone(const string &phone) {
    pLogMessage logRequest = pLogMessage(new LogMessage());
    logRequest->type = "lock";
    logRequest->message = "LOCK " + phone;

    log_info(phone, logRequest);

    Log::notice(logRequest);
}

void BlackList::log_unlock_phone(const string &phone) {
    pLogMessage logRequest = pLogMessage(new LogMessage());
    logRequest->type = "unlock";
    logRequest->message = "UNLOCK " + phone;

    log_info(phone, logRequest);

    Log::notice(logRequest);
}

void BlackList::log_info(const string &phone, pLogMessage &logRequest) {

    logRequest->params["number"] = phone;

    Repository repository;
    if (!repository.prepare() || !repository.billingData->ready()) {
        return;
    }

    auto serviceNumber = repository.getServiceNumber(atoll(phone.c_str()));
    if (serviceNumber == nullptr) {
        return;
    }

    logRequest->message += " / " + lexical_cast<string>(serviceNumber->client_account_id);
    logRequest->params["account_id"] = serviceNumber->client_account_id;

    auto client = repository.getAccount(serviceNumber->client_account_id);
    if (client == nullptr) {
        return;
    }

    double vat_rate = repository.getVatRate(client);

    double sumBalance = repository.billingData->statsAccountGetSumBalance(client->id, vat_rate);
    double sumDay = repository.billingData->statsAccountGetSumDay(client->id, vat_rate);

    auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
    double sumBalance2 = statsAccount2->getSumBalance(client->id, vat_rate);
    double sumDay2 = statsAccount2->getSumDay(client->id, vat_rate);

    double globalBalanceSum, globalDaySum;
    fetchGlobalCounters(repository, client->id, globalBalanceSum, globalDaySum, vat_rate);

    double spentBalanceSum, spentDaySum;
    spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
    spentDaySum = sumDay + sumDay2 + globalDaySum;

    logRequest->params["balance_stat"] = client->balance;
    logRequest->params["balance_local"] = sumBalance;
    logRequest->params["balance_current"] = sumBalance2;
    logRequest->params["balance_global"] = globalBalanceSum;
    logRequest->params["balance_realtime"] = client->balance + spentBalanceSum;
    if (client->hasCreditLimit()) {
        logRequest->params["credit_limit"] = client->credit;
        logRequest->params["credit_available"] = client->balance + client->credit + spentBalanceSum;
    }

    if (client->hasTermCreditLimit()) {
        logRequest->params["term_credit_limit"] = client->credit_term;
        logRequest->params["term_credit_available"] = client->balance + client->credit_term + spentBalanceSum;
    }

    logRequest->params["daily_local"] = sumDay;
    logRequest->params["daily_current"] = sumDay2;
    logRequest->params["daily_global"] = globalDaySum;
    logRequest->params["daily_total"] = spentDaySum;
    if (client->hasDailyLimit()) {
        logRequest->params["daily_limit"] = client->limit_d;
        logRequest->params["daily_available"] = client->limit_d + spentDaySum;
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

    if (client->isConsumedTermCreditLimit(spentBalanceSum)) {
        logRequest->params["block_term_credit_flag"] = "true";
    }

    if (client->isConsumedDailyLimit(spentDaySum)) {
        logRequest->params["block_daily_flag"] = "true";
    }

}

void BlackList::fetchGlobalCounters(Repository &repository, int accountId, double &globalBalanceSum, double &globalDaySum, double vat_rate) {

    GlobalCounters * globalCounter = nullptr;
    if (repository.data->globalCounters.ready()) {
        globalCounter = repository.data->globalCounters.get()->find(accountId);
    }

    if (globalCounter != nullptr) {
        globalBalanceSum = globalCounter->sumBalance(vat_rate);
        globalDaySum = globalCounter->sumDay(vat_rate);
    } else {
        globalBalanceSum = 0.0;
        globalDaySum = 0.0;
    }
}
