#include "ThreadBlacklist.h"
#include "../classes/AppBill.h"
#include "ThreadSelectGlobalCounters.h"

ThreadBlacklist::ThreadBlacklist() {
    id = idName();
    name = "Blacklist";

    loader = DataLoader::instance();
    blacklist_global = BlackListGlobal::instance();
    blacklist_local = BlackListLocal::instance();
    blacklist_trunk = BlackListTrunk::instance();

    last_sync_from_openca_time = 0;
}

bool ThreadBlacklist::ready() {
//    last_sync_from_openca_time = time(NULL);
    return app().init_sync_done &&
            app().init_load_data_done &&
            app().init_load_counters_done &&
            app().init_bill_runtime_started;
}

bool ThreadBlacklist::prepare() {
    if (!blacklist_local->fetch()) {
        return false;
    }

    if (!blacklist_global->fetch()) {
        return false;
    }

    if (!blacklist_trunk->fetch()) {
        //return false;
    }

    last_sync_from_openca_time = time(NULL);

    sync_blacklist();

    return true;
}

void ThreadBlacklist::run() {

    sync_once_per_hour();

    update_voip_auto_disabled();

}

void ThreadBlacklist::sync_once_per_hour() {
    if (last_sync_from_openca_time + app().conf.openca_sync_blacklist_interval_min >= time(NULL))
        return;
    
    if (loader->usageReloadTimestamp < last_sync_from_openca_time
            && last_sync_from_openca_time + app().conf.openca_sync_blacklist_interval_max >= time(NULL))
        return;
    
    last_sync_from_openca_time = time(NULL);

    blacklist_local->fetch();
    blacklist_global->fetch();
    blacklist_trunk->fetch();

    sync_blacklist();
}

static void delExpiredUsages(BlackList * blacklist, UsageObjList * usages) {
    map<long long int, time_t>::iterator i;

    i = blacklist->blacklist.begin();
    while (i != blacklist->blacklist.end()) {
        if (usages->find(i->first) == 0) {
            blacklist->del(i->first);
        }
        ++i;
    }
}

void ThreadBlacklist::sync_blacklist() {
    shared_ptr<UsageObjList> usages;

    {
        lock_guard<mutex> lock(loader->rwlock);
        usages = loader->usage.get(get_tday());
        if (usages == 0) return;
    }
    
    delExpiredUsages(blacklist_local, usages.get());
    delExpiredUsages(blacklist_global, usages.get());
    delExpiredUsages(blacklist_trunk, usages.get());

    blacklist_local->push();
    blacklist_global->push();
    blacklist_trunk->push();
}

static bool needLockLocal(pClientObj client, ClientCounterObj &cc, pGlobalCountersObj globalCounter, pUsageObj usage, FminCounter * counters_fmin) {
    if (client != 0) {
        double spentBalanceSum = cc.sumBalance() + (globalCounter ? globalCounter->sumBalance() : 0);
        int usedFreeSeconds = counters_fmin->get(usage->id, 1);

        if ((client->credit >= 0 && client->balance + client->credit - spentBalanceSum < 0) &&
            (client->last_payed_month < get_tmonth() || usedFreeSeconds >= usage->free_seconds)
        ) {
            return true;
        }
    }
    
    return false;
}

static bool needLockGlobal(pClientObj client, ClientCounterObj &cc, pGlobalCountersObj globalCounter) {
    if (client != 0) {
        double spentBalanceSum = cc.sumBalance() + (globalCounter ? globalCounter->sumBalance() : 0);
        double spentDaySum = cc.sumDay() + (globalCounter ? globalCounter->sumDay() : 0);
        double spentMonthSum = cc.sumMonth() + (globalCounter ? globalCounter->sumMonth() : 0);
        
        if ((client->credit >= 0 && client->balance + client->credit - spentBalanceSum < 0) ||
                (client->limit_d != 0 && client->limit_d - spentDaySum < 0) ||
                (client->limit_m != 0 && client->limit_m - spentMonthSum < 0) ||
                (client->disabled)
                ) {
            return true;
        }
    }
    
    return false;
}

void ThreadBlacklist::update_voip_auto_disabled() {

    shared_ptr<ClientObjList> clients;
    shared_ptr<UsageObjList> usages;

    {
        lock_guard<mutex> lock(loader->rwlock);

        clients = loader->client;
        usages = loader->usage.get(get_tday());
        if (usages == 0) return;
    }

    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        shared_ptr<ClientCounter> counters_clients = loader->counter_client;
        shared_ptr<FminCounter> counters_fmin = loader->counter_fmin.get(get_tmonth());
        shared_ptr<GlobalCountersObjList> counters_global = ThreadSelectGlobalCounters::getList();

        if (counters_clients == 0 || counters_fmin == 0 || counters_global == 0) return;

        for (int j = 0; j < usages->count; j++) {
            pUsageObj usage = (pUsageObj) usages->_get(j);
            pClientObj client = clients->find(usage->client_id);
            pGlobalCountersObj globalCounter = counters_global->find(usage->client_id);

            ClientCounterObj &cc = counters_clients->get(usage->client_id);

            bool need_lock_local = needLockLocal(client, cc, globalCounter, usage, counters_fmin.get());
            bool need_lock_global = needLockGlobal(client, cc, globalCounter);

            cc.updateVoipDisabledLocal(need_lock_local);
            cc.updateVoipDisabledGlobal(need_lock_global);

            if (usage->isConnectedOperator()) {
                blacklist_trunk->change_lock(usage->phone_num, need_lock_global);
            } else {
                blacklist_local->change_lock(usage->phone_num, need_lock_local);
                blacklist_global->change_lock(usage->phone_num, need_lock_global);
            }
        }
    }

    blacklist_local->push();
    blacklist_global->push();
    blacklist_trunk->push();
}

void ThreadBlacklist::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";


    shared_ptr<UsageObjList> usages;
    pUsageObj usage;

    {
        lock_guard<mutex> lock(loader->rwlock);
        usages = loader->usage.get(get_tday());
    }

    {
        lock_guard<mutex> lock(blacklist_local->rwlock);

        if (blacklist_local->blacklist.size() > 0) {
            html << "BlackListLocal: <b>" << blacklist_local->blacklist.size() << "</b><br/>\n";
            map<long long int, time_t>::iterator i = blacklist_local->blacklist.begin();
            while (i != blacklist_local->blacklist.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_local->list_to_add.size() > 0) {
            html << "BlackListLocal to Add: <b>" << blacklist_local->list_to_add.size() << "</b><br/>\n";
            map<long long int, bool>::iterator i = blacklist_local->list_to_add.begin();
            while (i != blacklist_local->list_to_add.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_local->list_to_del.size() > 0) {
            html << "BlackListLocal to Del: <b>" << blacklist_local->list_to_del.size() << "</b><br/>\n";
            map<long long int, bool>::iterator i = blacklist_local->list_to_del.begin();
            while (i != blacklist_local->list_to_del.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
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
            map<long long int, time_t>::iterator i = blacklist_global->blacklist.begin();
            while (i != blacklist_global->blacklist.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_global->list_to_add.size() > 0) {
            html << "BlackListGlobal to Add: <b>" << blacklist_global->list_to_add.size() << "</b><br/>\n";
            map<long long int, bool>::iterator i = blacklist_global->list_to_add.begin();
            while (i != blacklist_global->list_to_add.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_global->list_to_del.size() > 0) {
            html << "BlackListGlobal to Del: <b>" << blacklist_global->list_to_del.size() << "</b><br/>\n";
            map<long long int, bool>::iterator i = blacklist_global->list_to_del.begin();
            while (i != blacklist_global->list_to_del.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
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
            map<long long int, time_t>::iterator i = blacklist_trunk->blacklist.begin();
            while (i != blacklist_trunk->blacklist.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_trunk->list_to_add.size() > 0) {
            html << "BlackListTrunk to Add: <b>" << blacklist_trunk->list_to_add.size() << "</b><br/>\n";
            map<long long int, bool>::iterator i = blacklist_trunk->list_to_add.begin();
            while (i != blacklist_trunk->list_to_add.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        if (blacklist_trunk->list_to_del.size() > 0) {
            html << "BlackListTrunk to Del: <b>" << blacklist_trunk->list_to_del.size() << "</b><br/>\n";
            map<long long int, bool>::iterator i = blacklist_trunk->list_to_del.begin();
            while (i != blacklist_trunk->list_to_del.end()) {
                html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                html << "<b>" << lexical_cast<string>(i->first) << "</b>";
                if (usages != 0 && (usage = usages->find(i->first)) != 0)
                    html << " / " << "<a href='/client?id=" << usage->client_id << "'>" << usage->client_id << "</a>";
                html << "<br/>\n";
                ++i;
            }
        }

        html << "<br/>\n";
    }
}

