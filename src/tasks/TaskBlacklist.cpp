#include "TaskBlacklist.h"

TaskBlacklist::TaskBlacklist()
{
    id = "blacklist";
    name = "Blacklist";

    loader = DataLoader::instance();
    blacklist = BlackList::instance();
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
}

void TaskBlacklist::run()
{
    while(true){

        t.start();

        blacklist->fetch_once_per_day();

        update_voip_auto_disabled();

        t.stop();

        ssleep(1);

    }
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
        bool need_lock = false;

        pUsageObj usage = (pUsageObj)usages->_get(j);
        pClientObj client = clients->find(usage->client_id);
        ClientCounterObj &cc = counters_clients->get(usage->client_id);
        if (client != 0)
        {
            if (    (client->credit >= 0 && client->balance + client->credit - cc.sumBalance() < 0) ||
                    (client->limit_d != 0 && client->limit_d - cc.sumDay() < 0) ||
                    (client->limit_m != 0 && client->limit_m - cc.sumMonth() < 0) ||
                    (client->disabled)
            ){
                need_lock = true;
            }
        }

        if (cc.disabled != need_lock)
        {
            cc.disabled = need_lock;
            cc.updated = 1;
        }

        if (blacklist->is_locked(usage->phone_num) != need_lock)
        {
            if (need_lock)
            {
                blacklist->add(usage->phone_num);
            }else{
                blacklist->del(usage->phone_num);
            }
        }
    }
    loader->counter_rwlock.unlock();

    blacklist->push();
}


void TaskBlacklist::htmlfull(stringstream &html){
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() <<  "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

    blacklist->lock.lock();

    if (blacklist->blacklist.size() > 0)
    {
        html << "BlackList: <b>" << blacklist->blacklist.size() << "</b><br/>\n";
        map<long long int,time_t>::iterator i = blacklist->blacklist.begin();
        while (i != blacklist->blacklist.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist->list_to_add.size() > 0)
    {
        html << "BlackList to Add: <b>" << blacklist->list_to_add.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist->list_to_add.begin();
        while (i != blacklist->list_to_add.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            html << "<br/>\n";
            ++i;
        }
    }

    if (blacklist->list_to_del.size() > 0)
    {
        html << "BlackList to Del: <b>" << blacklist->list_to_del.size() << "</b><br/>\n";
        map<long long int,bool>::iterator i = blacklist->list_to_del.begin();
        while (i != blacklist->list_to_del.end()) {
            html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            html << lexical_cast<string>(i->first);
            html << "<br/>\n";
            ++i;
        }
    }

    html << "<br/>\n";
    blacklist->lock.unlock();
}

