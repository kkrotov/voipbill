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

    auto clientLock = repository.billingData->clientLock.get();

    for (int client_account_id : repository.activeCounter->activeClients) {

        bool need_lock_local = needLockLocal(client_account_id);
        bool need_lock_global = needLockGlobal(client_account_id);

        if (need_lock_local || need_lock_global) {
            ClientLockObj &client_lock = client_locks[client_account_id];
            client_lock.client_id = client_account_id;
            client_lock.disabled_local = need_lock_local;
            client_lock.disabled_global = need_lock_global;
        }
    }

    clientLock->push(client_locks);


}


bool ThreadClientLock::needLockLocal(int client_account_id) {

    auto client = repository.getAccount(client_account_id);
    if (client != nullptr) {

        if (client->disabled) {
            return true;
        }

        auto globalCounter = repository.data->globalCounters.get()->find(client->id);
        double vat_rate = repository.getVatRate(client);
        double sumBalance = repository.billingData->statsAccountGetSumBalance(client->id, vat_rate);
        double sumDay = repository.billingData->statsAccountGetSumDay(client->id, vat_rate);
        double spentBalanceSum = sumBalance + (globalCounter ? globalCounter->sumBalance(vat_rate) : 0.0);
        double spentDaySum = sumDay + (globalCounter ? globalCounter->sumDay(vat_rate) : 0.0);

        if (client->isConsumedCreditLimit(spentBalanceSum)) {
            return true;
        }

        if (client->isConsumedDailyLimit(spentDaySum)) {
            return true;
        }

    }

    return false;
}

bool ThreadClientLock::needLockGlobal(int client_account_id) {

    auto client = repository.getAccount(client_account_id);
    if (client != nullptr) {

        if (client->is_blocked) {
            return true;
        }

    }

    return false;
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
