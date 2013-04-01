#include "TaskBlacklist.h"

TaskBlacklist::TaskBlacklist()
{
    id = "blacklist";
    name = "Blacklist";

    loader = DataLoader::instance();
    blacklist = BlackListFull::instance();
    blacklist_global = BlackListGlobal::instance();
    blacklist_local = BlackListLocal::instance();

    last_sync_from_openca_time = 0;
}

void TaskBlacklist::wait()
{
    while(app.init_sync_done == false ||
          app.init_load_data_done == false ||
          app.init_load_counters_done == false ||
          app.init_bill_runtime_started == false
          )
    {
        ssleep(1);
    }
}

void TaskBlacklist::prepare()
{
    while(blacklist->fetch() == false)
    {
        ssleep(10);
    }

    while(blacklist_local->fetch() == false)
    {
        ssleep(10);
    }

    while(blacklist_global->fetch() == false)
    {
        ssleep(10);
    }

    last_sync_from_openca_time = time(NULL);

    sync_blacklist();
}

void TaskBlacklist::run()
{
    while(true){

        t.start();

        sync_once_per_day();

        update_voip_auto_disabled();

        t.stop();

        ssleep(1);

    }
}

void TaskBlacklist::sync_once_per_day()
{
    if (last_sync_from_openca_time + 86400 >= time(NULL))
        return;

    last_sync_from_openca_time = time(NULL);

    blacklist->fetch();
    blacklist_local->fetch();
    blacklist_global->fetch();

    sync_blacklist();
}


void TaskBlacklist::sync_blacklist()
{
    loader->rwlock.lock();

    shared_ptr<UsageObjList> usages = loader->usage.get(get_tday());
    if (usages == 0)
    {
        loader->rwlock.unlock();
        return;
    }

    loader->rwlock.unlock();


    map<long long int,time_t>::iterator i;
    i = blacklist->blacklist.begin();
    while (i != blacklist->blacklist.end()) {
        if (usages->find(i->first) == 0)
        {
            blacklist->del(i->first);
        }
        ++i;
    }

    i = blacklist_local->blacklist.begin();
    while (i != blacklist_local->blacklist.end()) {
        if (usages->find(i->first) == 0)
        {
            blacklist_local->del(i->first);
        }
        ++i;
    }

    i = blacklist_global->blacklist.begin();
    while (i != blacklist_global->blacklist.end()) {
        if (usages->find(i->first) == 0)
        {
            blacklist_global->del(i->first);
        }
        ++i;
    }

    blacklist->push();
    blacklist_local->push();
    blacklist_global->push();
}



void TaskBlacklist::update_voip_auto_disabled(){

    loader->rwlock.lock();

    shared_ptr<ClientObjList> clients = loader->client;

    shared_ptr<UsageObjList> usages = loader->usage.get(get_tday());
    if (usages == 0)
    {
        loader->rwlock.unlock();
        return;
    }

    loader->rwlock.unlock();

    loader->counter_rwlock.lock();
    shared_ptr<ClientCounter> counters_clients = loader->counter_client;
    if (counters_clients == 0)
    {
        loader->counter_rwlock.unlock();
        return;
    }

    for(int j=0;j<usages->count;j++)
    {
        bool need_lock_full = false;
        bool need_lock_local = false;
        bool need_lock_global = false;

        pUsageObj usage = (pUsageObj)usages->_get(j);
        pClientObj client = clients->find(usage->client_id);
        ClientCounterObj &cc = counters_clients->get(usage->client_id);
        if (client != 0)
        {
            if (    (client->credit >= 0 && client->balance + client->credit - cc.sumBalance() < 0) &&
                    (client->last_payed_month < get_tmonth())
            ){
                need_lock_local = true;
            }

            if (    (client->credit >= 0 && client->balance + client->credit - cc.sumBalance() < 0) ||
                    (client->limit_d != 0 && client->limit_d - cc.sumDay() < 0) ||
                    (client->limit_m != 0 && client->limit_m - cc.sumMonth() < 0) ||
                    (client->disabled)
            ){
                need_lock_global = true;
                //need_lock_full = true;
            }

        }

        if (cc.disabled_global != need_lock_global)
        {
            cc.disabled_global = need_lock_global;
            cc.updated = 1;
        }

        if (cc.disabled_local != need_lock_local)
        {
            cc.disabled_local = need_lock_local;
            cc.updated = 1;
        }

        if (blacklist->is_locked(usage->phone_num) != need_lock_full)
        {
            if (need_lock_full)
            {
                blacklist->add(usage->phone_num);
            }else{
                blacklist->del(usage->phone_num);
            }
        }

        if (blacklist_local->is_locked(usage->phone_num) != need_lock_local)
        {
            if (need_lock_local)
            {
                blacklist_local->add(usage->phone_num);
            }else{
                blacklist_local->del(usage->phone_num);
            }
        }

        if (blacklist_global->is_locked(usage->phone_num) != need_lock_global)
        {
            if (need_lock_global)
            {
                blacklist_global->add(usage->phone_num);
            }else{
                blacklist_global->del(usage->phone_num);
            }
        }

    }
    loader->counter_rwlock.unlock();

    blacklist->push();
    blacklist_local->push();
    blacklist_global->push();
}


void TaskBlacklist::htmlfull(stringstream &html){
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() <<  "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";


    loader->rwlock.lock();

    shared_ptr<UsageObjList> usages = loader->usage.get(get_tday());

    loader->rwlock.unlock();


    blacklist->lock.lock();

    if (blacklist->blacklist.size() > 0)
    {
        html << "BlackListFull: <b>" << blacklist->blacklist.size() << "</b><br/>\n";
        map<long long int,time_t>::iterator i = blacklist->blacklist.begin();
        while (i != blacklist->blacklist.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist->list_to_add.size() > 0)
    {
        html << "BlackListFull to Add: <b>" << blacklist->list_to_add.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist->list_to_add.begin();
        while (i != blacklist->list_to_add.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist->list_to_del.size() > 0)
    {
        html << "BlackListFull to Del: <b>" << blacklist->list_to_del.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist->list_to_del.begin();
        while (i != blacklist->list_to_del.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    html << "<br/>\n";
    blacklist->lock.unlock();



    blacklist_local->lock.lock();

    if (blacklist_local->blacklist.size() > 0)
    {
        html << "BlackListLocal: <b>" << blacklist_local->blacklist.size() << "</b><br/>\n";
        map<long long int,time_t>::iterator i = blacklist_local->blacklist.begin();
        while (i != blacklist_local->blacklist.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist_local->list_to_add.size() > 0)
    {
        html << "BlackListLocal to Add: <b>" << blacklist_local->list_to_add.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist_local->list_to_add.begin();
        while (i != blacklist_local->list_to_add.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist_local->list_to_del.size() > 0)
    {
        html << "BlackListLocal to Del: <b>" << blacklist_local->list_to_del.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist_local->list_to_del.begin();
        while (i != blacklist_local->list_to_del.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    html << "<br/>\n";
    blacklist_local->lock.unlock();



    blacklist_global->lock.lock();

    if (blacklist_global->blacklist.size() > 0)
    {
        html << "BlackListGlobal: <b>" << blacklist_global->blacklist.size() << "</b><br/>\n";
        map<long long int,time_t>::iterator i = blacklist_global->blacklist.begin();
        while (i != blacklist_global->blacklist.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist_global->list_to_add.size() > 0)
    {
        html << "BlackListGlobal to Add: <b>" << blacklist_global->list_to_add.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist_global->list_to_add.begin();
        while (i != blacklist_global->list_to_add.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist_global->list_to_del.size() > 0)
    {
        html << "BlackListGlobal to Del: <b>" << blacklist_global->list_to_del.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist_global->list_to_del.begin();
        while (i != blacklist_global->list_to_del.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            if (usages && usages->find(i->first) == 0)
                html << " -- del";
            html << "<br/>\n";
            ++i;
        }
    }

    html << "<br/>\n";
    blacklist_global->lock.unlock();
}

