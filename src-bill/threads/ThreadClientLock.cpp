#include "ThreadClientLock.h"

ThreadClientLock::ThreadClientLock() {
    id = idName();
    name = "Client lock";

}

bool ThreadClientLock::ready() {
    if (!repository.billingData->ready()) {
        return false;
    }

    if (!repository.prepare()) {
        return false;
    }

    return true;
}

void ThreadClientLock::run() {

    if (!repository.prepare()) {
        return;
    }

    if (!repository.billingData->ready()) {
        return;
    }

    if (!repository.data->globalCounters.ready()) {
        return;
    }


    map<int, ClientLockObj> client_locks;

    auto clientCounter = repository.billingData->clientCounter.get();
    auto clientLock = repository.billingData->clientLock.get();

    for (int client_account_id : repository.activeCounter->activeClients) {

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

    auto client = repository.getAccount(client_account_id);
    if (client != nullptr) {
        double vat_rate = repository.getVatRate(client);

        auto globalCounter = repository.data->globalCounters.get()->find(client->id);

        double spentBalanceSum = cc.sumBalance(vat_rate) + (globalCounter ? globalCounter->sumBalance(vat_rate) : 0.0);

        if (client->isConsumedCreditLimit(spentBalanceSum) && client->last_payed_month < get_tmonth(time(nullptr))) {
            result = true;
        }
    }

    return result;
}

bool ThreadClientLock::needLockGlobal(ClientCounterObj &cc, int client_account_id) {
    bool result = false;

    auto client = repository.getAccount(client_account_id);
    if (client != nullptr) {
        double vat_rate = repository.getVatRate(client);

        auto globalCounter = repository.data->globalCounters.get()->find(client->id);

        double spentBalanceSum = cc.sumBalance(vat_rate) + (globalCounter ? globalCounter->sumBalance(vat_rate) : 0.0);
        double spentDaySum = cc.sumDay(vat_rate) + (globalCounter ? globalCounter->sumDay(vat_rate) : 0.0);
        double spentMonthSum = cc.sumMonth(vat_rate) + (globalCounter ? globalCounter->sumMonth(vat_rate) : 0.0);

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


bool ThreadClientLock::hasFullHtml() {
    return true;
}


void ThreadClientLock::htmlfull(stringstream &html) {
    this->html(html);

    if (!repository.prepare()) {
        return;
    }



    html << "<table border=1>\n";
    html << "<tr>";
    html << "<th>Active client</th>";
    html << "<th>Numbers</th>";
    html << "<th>Trunks</th>";
    html << "</tr>\n";
    for (int clientId : repository.activeCounter->activeClients) {
        html << "<tr>\n";
        html << "<td align=left valign=top>" << clientId << "</td>\n";
        html << "<td valign=top>\n";
        {
            map<int, ServiceNumber> numbers = repository.activeCounter->clientNumbers[clientId];
            html << "<table>\n";
            html << "<tr>";
            html << "<th align=left>Service ID</th>";
            html << "<th align=left>DID</th>";
            html << "</tr>\n";
            for (auto it: numbers) {
                html << "<tr>";
                html << "<td align=left>" << it.second.id << "</td>";
                html << "<td align=left>" << it.second.did << "</td>";
                html << "</tr>\n";
            }
            html << "</table>\n";
            html << "&nbsp;";
        }
        html << "</td>\n";
        html << "<td valign=top>\n";
        {
            map<int, ServiceTrunk> trunks = repository.activeCounter->clientTrunks[clientId];
            html << "<table>\n";
            html << "<tr>";
            html << "<th align=left>Service ID</th>";
            html << "<th align=left>Trunk ID</th>";
            html << "<th align=left>Trunk Name</th>";
            html << "</tr>\n";
            for (auto it: trunks) {
                Trunk * trunk = repository.getTrunk(it.second.trunk_id);
                html << "<tr>";
                html << "<td align=left>" << it.second.id << "</td>";
                html << "<td align=left>" << it.second.trunk_id << "</td>";
                html << "<td align=left>" << (trunk != nullptr ? (const char *)&trunk->name[0] : "")  << "</td>";
                html << "</tr>\n";
            }
            html << "</table>\n";
            html << "&nbsp;";
        }
        html << "</td>\n";
        html << "</tr>\n";
    }
    html << "</table>\n";

}
