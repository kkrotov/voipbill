#include "ThreadBlacklist.h"
#include "../classes/AppBill.h"

ThreadBlacklist::ThreadBlacklist() {
    id = idName();
    name = "Blacklist";

    data = DataContainer::instance();
    billingData = DataBillingContainer::instance();

    blacklist_global = BlackListGlobal::instance();
    blacklist_local = BlackListLocal::instance();
    blacklist_trunk = BlackListTrunk::instance();

    last_sync_from_openca_time = 0;
}

bool ThreadBlacklist::ready() {
    last_sync_from_openca_time = time(NULL);


    if (!billingData->ready()) {
        return false;
    }

    return true;
}

bool ThreadBlacklist::prepare() {

    if (!data->prepareData(preparedData, time(nullptr))) {
        return false;
    }

    if (!blacklist_local->fetch()) {
        return false;
    }

    if (!blacklist_global->fetch()) {
        return false;
    }

    if (!blacklist_trunk->fetch()) {
        return false;
    }

    last_sync_from_openca_time = time(NULL);

    sync_blacklist();

    return true;
}

void ThreadBlacklist::run() {

    if (!data->prepareData(preparedData, time(nullptr))) {
        return;
    }

    if (!billingData->ready()) {
        return;
    }

    if (!data->globalCounters.ready()) {
        return;
    }


    sync_once_per_hour();

    update_voip_auto_disabled();

}

void ThreadBlacklist::sync_once_per_hour() {
    if (last_sync_from_openca_time + app().conf.openca_sync_blacklist_interval_min >= time(NULL))
        return;
    
    if (last_sync_from_openca_time + app().conf.openca_sync_blacklist_interval_max >= time(NULL))
        return;
    
    last_sync_from_openca_time = time(NULL);

    blacklist_local->fetch();
    blacklist_global->fetch();
    blacklist_trunk->fetch();

    sync_blacklist();
}

void ThreadBlacklist::delExpiredServiceNumber(BlackList * blacklist) {
    auto i = blacklist->blacklist.begin();
    while (i != blacklist->blacklist.end()) {
        if (preparedData.serviceNumber->find(atoll(i->first.c_str()), time(nullptr)) == 0) {
            blacklist->del(i->first);
        }
        ++i;
    }
}

void ThreadBlacklist::delExpiredServiceTrunk(BlackList * blacklist) {
    auto i = blacklist->blacklist.begin();
    while (i != blacklist->blacklist.end()) {
        if (preparedData.trunkByName->find(i->first.c_str()) == 0) {
            blacklist->del(i->first);
        }
        ++i;
    }
}

void ThreadBlacklist::sync_blacklist() {

    delExpiredServiceNumber(blacklist_local);
    delExpiredServiceNumber(blacklist_global);
    delExpiredServiceTrunk(blacklist_trunk);

    blacklist_local->push();
    blacklist_global->push();
    blacklist_trunk->push();
}

bool ThreadBlacklist::needLockLocal(ServiceNumber * serviceNumber) {
    bool result = false;

    ClientCounterObj &cc = billingData->clientCounter.get()->get(serviceNumber->client_account_id);

    auto client = preparedData.client->find(serviceNumber->client_account_id);
    if (client != nullptr) {
        auto globalCounter = data->globalCounters.get()->find(client->id);

        double spentBalanceSum = cc.sumBalance() + (globalCounter ? globalCounter->sumBalance() : 0);

        if (client->isConsumedCreditLimit(spentBalanceSum) && client->last_payed_month < get_tmonth()) {
            result = true;
        }
    }

    cc.updateVoipDisabledLocal(result);

    return result;
}

bool ThreadBlacklist::needLockGlobal(ServiceNumber * serviceNumber) {
    bool result = false;

    ClientCounterObj &cc = billingData->clientCounter.get()->get(serviceNumber->client_account_id);

    auto client = preparedData.client->find(serviceNumber->client_account_id);
    if (client != nullptr) {
        auto globalCounter = data->globalCounters.get()->find(client->id);

        double spentBalanceSum = cc.sumBalance() + (globalCounter ? globalCounter->sumBalance() : 0);
        double spentDaySum = cc.sumDay() + (globalCounter ? globalCounter->sumDay() : 0);
        double spentMonthSum = cc.sumMonth() + (globalCounter ? globalCounter->sumMonth() : 0);

        if (client->isConsumedCreditLimit(spentBalanceSum) ||
            client->isConsumedDailyLimit(spentDaySum) ||
            client->isConsumedMonthlyLimit(spentMonthSum) ||
            client->disabled
                ) {
            result = true;
        }
    }

    cc.updateVoipDisabledGlobal(result);

    return result;
}

bool ThreadBlacklist::needLockTrunk(ServiceTrunk * serviceTrunk) {
    bool result = false;

    ClientCounterObj &cc = billingData->clientCounter.get()->get(serviceTrunk->client_account_id);

    auto client = preparedData.client->find(serviceTrunk->client_account_id);
    if (client != nullptr) {
        auto globalCounter = data->globalCounters.get()->find(client->id);

        double spentBalanceSum = cc.sumBalance() + (globalCounter ? globalCounter->sumBalance() : 0);
        double spentDaySum = cc.sumDay() + (globalCounter ? globalCounter->sumDay() : 0);
        double spentMonthSum = cc.sumMonth() + (globalCounter ? globalCounter->sumMonth() : 0);

        if (client->isConsumedCreditLimit(spentBalanceSum) ||
            client->isConsumedDailyLimit(spentDaySum) ||
            client->isConsumedMonthlyLimit(spentMonthSum) ||
            client->disabled
                ) {
            result = true;
        }
    }

    cc.updateVoipDisabledLocal(result);
    cc.updateVoipDisabledGlobal(result);

    return result;
}

void ThreadBlacklist::update_voip_auto_disabled() {

    time_t now = time(nullptr);

    for (size_t j = 0; j < preparedData.serviceNumber->size(); j++) {
        auto serviceNumber = preparedData.serviceNumber->get(j);
        if (serviceNumber->activation_dt > now || serviceNumber->expire_dt < now) {
            continue;
        }

        bool need_lock_local = needLockLocal(serviceNumber);
        bool need_lock_global = needLockGlobal(serviceNumber);

        blacklist_local->change_lock(serviceNumber->did, need_lock_local);
        blacklist_global->change_lock(serviceNumber->did, need_lock_global);
    }

    for (size_t j = 0; j < preparedData.serviceTrunk->size(); j++) {
        auto serviceTrunk = preparedData.serviceTrunk->get(j);
        if (serviceTrunk->activation_dt > now || serviceTrunk->expire_dt < now) {
            continue;
        }

        bool need_lock_trunk = needLockTrunk(serviceTrunk);

        auto trunk = preparedData.trunk->find(serviceTrunk->trunk_id);
        if (trunk != nullptr) {
            blacklist_trunk->change_lock(trunk->trunk_name, need_lock_trunk);
        }
    }

    blacklist_local->push();
    blacklist_global->push();
    blacklist_trunk->push();
}

void ThreadBlacklist::htmlfull(stringstream &html) {
    this->html(html);

    if (!data->prepareData(preparedData, time(nullptr))) {
        return;
    }

    auto usages = preparedData.serviceNumber;
    ServiceNumber * usage;
    {
        lock_guard<mutex> lock(blacklist_local->rwlock);

        if (blacklist_local->blacklist.size() > 0) {
            html << "BlackListLocal: <b>" << blacklist_local->blacklist.size() << "</b><br/>\n";
            auto i = blacklist_local->blacklist.begin();
            while (i != blacklist_local->blacklist.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                if ((usage = usages->find(atoll(i->first.c_str()), time(nullptr))) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_local->list_to_add.size() > 0) {
            html << "BlackListLocal to Add: <b>" << blacklist_local->list_to_add.size() << "</b><br/>\n";
            auto i = blacklist_local->list_to_add.begin();
            while (i != blacklist_local->list_to_add.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                if ((usage = usages->find(atoll(i->first.c_str()), time(nullptr))) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_local->list_to_del.size() > 0) {
            html << "BlackListLocal to Del: <b>" << blacklist_local->list_to_del.size() << "</b><br/>\n";
            auto i = blacklist_local->list_to_del.begin();
            while (i != blacklist_local->list_to_del.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                if ((usage = usages->find(atoll(i->first.c_str()), time(nullptr))) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        html << "<br/>\n";
    }


    {
        lock_guard<mutex> lock(blacklist_global->rwlock);

        if (blacklist_global->blacklist.size() > 0) {
            html << "BlackListGlobal: <b>" << blacklist_global->blacklist.size() << "</b><br/>\n";
            auto i = blacklist_global->blacklist.begin();
            while (i != blacklist_global->blacklist.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                if ((usage = usages->find(atoll(i->first.c_str()), time(nullptr))) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_global->list_to_add.size() > 0) {
            html << "BlackListGlobal to Add: <b>" << blacklist_global->list_to_add.size() << "</b><br/>\n";
            auto i = blacklist_global->list_to_add.begin();
            while (i != blacklist_global->list_to_add.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                if ((usage = usages->find(atoll(i->first.c_str()), time(nullptr))) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_global->list_to_del.size() > 0) {
            html << "BlackListGlobal to Del: <b>" << blacklist_global->list_to_del.size() << "</b><br/>\n";
            auto i = blacklist_global->list_to_del.begin();
            while (i != blacklist_global->list_to_del.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                if ((usage = usages->find(atoll(i->first.c_str()), time(nullptr))) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        html << "<br/>\n";
    }

    {
        lock_guard<mutex> lock(blacklist_trunk->rwlock);

        if (blacklist_trunk->blacklist.size() > 0) {
            html << "BlackListTrunk: <b>" << blacklist_trunk->blacklist.size() << "</b><br/>\n";
            auto i = blacklist_trunk->blacklist.begin();
            while (i != blacklist_trunk->blacklist.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                auto trunk = preparedData.trunkByName->find(i->first.c_str());
                if (trunk == nullptr) {
                    trunk = preparedData.trunkByAlias->find(i->first.c_str());
                }
                if (trunk != nullptr) {
                    auto serviceTrunk = preparedData.serviceTrunk->find(trunk->id, time(nullptr));
                    if (serviceTrunk != nullptr)
                        html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                }
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_trunk->list_to_add.size() > 0) {
            html << "BlackListTrunk to Add: <b>" << blacklist_trunk->list_to_add.size() << "</b><br/>\n";
            auto i = blacklist_trunk->list_to_add.begin();
            while (i != blacklist_trunk->list_to_add.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                auto trunk = preparedData.trunkByName->find(i->first.c_str());
                if (trunk == nullptr) {
                    trunk = preparedData.trunkByAlias->find(i->first.c_str());
                }
                if (trunk != nullptr) {
                    auto serviceTrunk = preparedData.serviceTrunk->find(trunk->id, time(nullptr));
                    if (serviceTrunk != nullptr)
                        html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                }
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_trunk->list_to_del.size() > 0) {
            html << "BlackListTrunk to Del: <b>" << blacklist_trunk->list_to_del.size() << "</b><br/>\n";
            auto i = blacklist_trunk->list_to_del.begin();
            while (i != blacklist_trunk->list_to_del.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << i->first << "</b>";
                auto trunk = preparedData.trunkByName->find(i->first.c_str());
                if (trunk == nullptr) {
                    trunk = preparedData.trunkByAlias->find(i->first.c_str());
                }
                if (trunk != nullptr) {
                    auto serviceTrunk = preparedData.serviceTrunk->find(trunk->id, time(nullptr));
                    if (serviceTrunk != nullptr)
                        html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                }
                html << "<br/>\n";
                ++i;
            }
        }

        html << "<br/>\n";
    }

}

