#include "ThreadBlacklist.h"
#include "../classes/AppBill.h"
#include "ThreadSelectGlobalCounters.h"

ThreadBlacklist::ThreadBlacklist() {
    id = "blacklist";
    name = "Blacklist";

    loader = DataLoader::instance();
    blacklist_global = BlackListGlobal::instance();
    blacklist_local = BlackListLocal::instance();

    last_sync_from_openca_time = 0;
}

bool ThreadBlacklist::ready() {
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

    last_sync_from_openca_time = time(NULL);

    sync_blacklist();

    return true;
}

void ThreadBlacklist::run() {

    sync_once_per_day();

    update_voip_auto_disabled();

}

void ThreadBlacklist::sync_once_per_day() {
    if (last_sync_from_openca_time + 86400 >= time(NULL))
        return;

    last_sync_from_openca_time = time(NULL);

    blacklist_local->fetch();
    blacklist_global->fetch();

    sync_blacklist();
}

void ThreadBlacklist::sync_blacklist() {
    shared_ptr<UsageObjList> usages;

    {
        lock_guard<mutex> lock(loader->rwlock);
        usages = loader->usage.get(get_tday());
        if (usages == 0) return;
    }

    map<long long int, time_t>::iterator i;

    i = blacklist_local->blacklist.begin();
    while (i != blacklist_local->blacklist.end()) {
        if (usages->find(i->first) == 0) {
            blacklist_local->del(i->first);
        }
        ++i;
    }

    i = blacklist_global->blacklist.begin();
    while (i != blacklist_global->blacklist.end()) {
        if (usages->find(i->first) == 0) {
            blacklist_global->del(i->first);
        }
        ++i;
    }

    blacklist_local->push();
    blacklist_global->push();
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
        if (counters_clients == 0 || counters_fmin == 0) return;

        shared_ptr<GlobalCountersObjList> globalCounters = ThreadSelectGlobalCounters::getList();

        for (int j = 0; j < usages->count; j++) {
            bool need_lock_local = false;
            bool need_lock_global = false;

            pUsageObj usage = (pUsageObj) usages->_get(j);
            pClientObj client = clients->find(usage->client_id);
            ClientCounterObj &cc = counters_clients->get(usage->client_id);

            double spentBalanceSum = cc.sumBalance();
            double spentDaySum = cc.sumDay();
            double spentMonthSum = cc.sumMonth();

            pGlobalCountersObj globalCounter = globalCounters->find(usage->client_id);
            if (globalCounter) {
                spentBalanceSum += globalCounter->sumBalance();
                spentDaySum += globalCounter->sumDay();
                spentMonthSum += globalCounter->sumMonth();
            }

            int used_free_seconds = counters_fmin->get(usage->id, 1);
            if (client != 0) {
                if ((client->credit >= 0 && client->balance + client->credit - spentBalanceSum < 0) &&
                        (client->last_payed_month < get_tmonth() || used_free_seconds >= usage->free_seconds)
                        ) {
                    need_lock_local = true;
                }

                if ((client->credit >= 0 && client->balance + client->credit - spentBalanceSum < 0) ||
                        (client->limit_d != 0 && client->limit_d - spentDaySum < 0) ||
                        (client->limit_m != 0 && client->limit_m - spentMonthSum < 0) ||
                        (client->disabled)
                        ) {
                    need_lock_global = true;
                }

            }

            if (cc.disabled_global != need_lock_global) {
                cc.disabled_global = need_lock_global;
                cc.updated = 1;
            }

            if (cc.disabled_local != need_lock_local) {
                cc.disabled_local = need_lock_local;
                cc.updated = 1;
            }

            if (blacklist_local->is_locked(usage->phone_num) != need_lock_local) {
                if (need_lock_local) {
                    blacklist_local->add(usage->phone_num);
                } else {
                    blacklist_local->del(usage->phone_num);
                }
            }

            if (blacklist_global->is_locked(usage->phone_num) != need_lock_global) {
                if (need_lock_global) {
                    blacklist_global->add(usage->phone_num);
                } else {
                    blacklist_global->del(usage->phone_num);
                }
            }

        }
    }

    blacklist_local->push();
    blacklist_global->push();
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
        lock_guard<mutex> lock(blacklist_local->lock);

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
        lock_guard<mutex> lock(blacklist_global->lock);

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
}

