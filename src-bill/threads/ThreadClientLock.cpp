#include "ThreadClientLock.h"

ThreadClientLock::ThreadClientLock() {
    id = idName();
    name = "Client lock";

    data = DataContainer::instance();
    billingData = DataBillingContainer::instance();
}

bool ThreadClientLock::ready() {
    if (!billingData->ready()) {
        return false;
    }

    if (!data->prepareData(preparedData, time(nullptr))) {
        return false;
    }

    return true;
}

void ThreadClientLock::run() {

    time_t now = time(nullptr);

    if (!data->prepareData(preparedData, time(nullptr))) {
        return;
    }

    if (!billingData->ready()) {
        return;
    }

    if (!data->globalCounters.ready()) {
        return;
    }


    map<int, ClientLockObj> client_locks;

    auto clientCounter = billingData->clientCounter.get();
    auto clientLock = billingData->clientLock.get();

    for (int client_account_id : preparedData.activeCounter->activeClients) {

        ClientCounterObj cc = clientCounter->get(client_account_id);

        bool need_lock_local = needLockLocal(cc, client_account_id);
        bool need_lock_global = needLockGlobal(cc, client_account_id);

        if (need_lock_local || need_lock_global) {
            ClientLockObj &client_lock = client_locks[client_account_id];
            client_lock.client_id = client_account_id;
            client_lock.disabled_local = need_lock_local;
            client_lock.disabled_global = need_lock_global;
        }
    }

    clientLock->push(client_locks);


}


bool ThreadClientLock::needLockLocal(ClientCounterObj &cc, int client_account_id) {
    bool result = false;

    auto client = preparedData.client->find(client_account_id);
    if (client != nullptr) {
        auto globalCounter = data->globalCounters.get()->find(client->id);

        double spentBalanceSum = cc.sumBalance() + (globalCounter ? globalCounter->sumBalance() : 0.0);

        if (client->isConsumedCreditLimit(spentBalanceSum) && client->last_payed_month < get_tmonth()) {
            result = true;
        }
    }

    return result;
}

bool ThreadClientLock::needLockGlobal(ClientCounterObj &cc, int client_account_id) {
    bool result = false;

    auto client = preparedData.client->find(client_account_id);
    if (client != nullptr) {
        auto globalCounter = data->globalCounters.get()->find(client->id);

        double spentBalanceSum = cc.sumBalance() + (globalCounter ? globalCounter->sumBalance() : 0.0);
        double spentDaySum = cc.sumDay() + (globalCounter ? globalCounter->sumDay() : 0.0);
        double spentMonthSum = cc.sumMonth() + (globalCounter ? globalCounter->sumMonth() : 0.0);

        if (client->isConsumedCreditLimit(spentBalanceSum) ||
            client->isConsumedDailyLimit(spentDaySum) ||
            client->isConsumedMonthlyLimit(spentMonthSum) ||
            client->disabled
                ) {
            result = true;
        }
    }

    return result;
}
