#include "BlackList.h"

#include "UdpControlClient.h"
#include "Log.h"
#include "DataLoader.h"
#include "../common.h"


BlackList::BlackList()
{
    last_fetch_from_openca_time = 0;
}

void BlackList::add(long long int phone)
{
    lock.lock();
    if (blacklist.find(phone) == blacklist.end())
    {
        list_to_add[phone] = true;
    }
    if (list_to_del.find(phone) != list_to_del.end())
    {
        list_to_del.erase(phone);
    }
    lock.unlock();
}

void BlackList::del(long long int phone)
{
    lock.lock();
    if (blacklist.find(phone) != blacklist.end())
    {
        list_to_del[phone] = true;
    }
    if (list_to_add.find(phone) != list_to_add.end())
    {
        list_to_add.erase(phone);
    }
    lock.unlock();
}

bool BlackList::fetch_once_per_day()
{
    if (last_fetch_from_openca_time + 86400 < time(NULL))
    {
        return fetch();
    }else{
        return true;
    }
}

bool BlackList::fetch()
{
    vector<string> curr_list;
    if (UdpControlClient::blacklist(curr_list) == false)
    {
        Log::er("Can not fetch black list from opanca");
        return false;
    }

    last_fetch_from_openca_time = time(NULL);

    lock.lock();

    {
        vector<string>::iterator i = curr_list.begin();
        while (i != curr_list.end()) {
            long long int phone = atoll(((string)*i).c_str());
            blacklist[phone] = last_fetch_from_openca_time;
            ++i;
        }
    }

    {
        map<long long int,time_t>::iterator i = blacklist.begin();
        while (i != blacklist.end()) {
            if (i->second != last_fetch_from_openca_time)
            {
                blacklist.erase(i++);
            }else
                ++i;
        }
    }


    {
        map<long long int,bool>::iterator i = list_to_add.begin();
        while (i != list_to_add.end()) {
            if (blacklist.find(i->first) != blacklist.end())
            {
                list_to_add.erase(i++);
            }else
                ++i;
        }
    }

    {
        map<long long int,bool>::iterator i = list_to_del.begin();
        while (i != list_to_del.end()) {
            if (blacklist.find(i->first) == blacklist.end())
            {
                list_to_del.erase(i++);
            }else
                ++i;
        }
    }

    lock.unlock();
    return true;
}

void BlackList::push()
{
    {
        lock.lock();
        vector<long long int> list;
        map<long long int,bool>::iterator i = list_to_add.begin();
        while (i != list_to_add.end()) {
            list.push_back(i->first);
             ++i;
        }
        lock.unlock();

        vector<long long int>::iterator ii = list.begin();
        while (ii != list.end()) {
            string phone = lexical_cast<string>(*ii);
            if (UdpControlClient::lock(phone) == false){
                Log::er("Can not lock phone "+phone);
                ++ii;
                continue;
            }
            lock.lock();
            if (blacklist.find(*ii) == blacklist.end())
            {
                blacklist[*ii] = time(NULL);
            }
            if (list_to_add.find(*ii) != list_to_add.end())
            {
                list_to_add.erase(*ii);
            }
            lock.unlock();

            log_lock_phone(phone);

            ++ii;
        }
    }

    {
        lock.lock();
        vector<long long int> list;
        map<long long int,bool>::iterator i = list_to_del.begin();
        while (i != list_to_del.end()) {
            list.push_back(i->first);
             ++i;
        }
        lock.unlock();

        vector<long long int>::iterator ii = list.begin();
        while (ii != list.end()) {
            string phone = lexical_cast<string>(*ii);
            if (UdpControlClient::unlock(phone) == false){
                Log::er("Can not unlock phone "+phone);
                ++ii;
                continue;
            }
            lock.lock();
            if (blacklist.find(*ii) != blacklist.end())
            {
                blacklist.erase(*ii);
            }
            if (list_to_del.find(*ii) != list_to_del.end())
            {
                list_to_del.erase(*ii);
            }
            lock.unlock();

            log_unlock_phone(phone);

            ++ii;
        }
    }

}

bool BlackList::is_locked(long long int phone)
{
    if (blacklist.find(phone) != blacklist.end())
    {
        if (list_to_del.find(phone) != list_to_del.end())
        {
            return false;
        }else{
            return true;
        }
    }else{
        if (list_to_add.find(phone) != list_to_add.end())
        {
            return true;
        }else{
            return false;
        }
    }
}

void BlackList::log_lock_phone(string &phone)
{
    string str = "LOCK " + phone;

    DataLoader *loader = DataLoader::instance();
    loader->rwlock.lock();
    shared_ptr<UsageObjList> usages_ptr = loader->usage.get(get_tday());
    shared_ptr<ClientObjList> clients_ptr = loader->client;
    loader->rwlock.unlock();

    UsageObjList *usages = usages_ptr.get();
    ClientObjList *clients = clients_ptr.get();

    if (usages != 0 && clients != 0)
    {
        pUsageObj usage = usages->find(phone.c_str());
        pClientObj client;
        if (usage != 0)
        {
            str = str + " / " + lexical_cast<string>(usage->client_id);
            client = clients->find(usage->client_id);
        }else{
            client = 0;
        }

        if (client != 0)
        {
            loader->counter_rwlock.lock();
            shared_ptr<ClientCounter> clients_counters_ptr = loader->counter_client;
            ClientCounter *clients_counters = clients_counters_ptr.get();
            if (clients_counters != 0)
            {
                ClientCounterObj &client_counter = clients_counters->get(usage->client_id);
                double sum_month = client_counter.sumMonth();
                double sum_day = client_counter.sumDay();
                double sum_balance = client_counter.sumBalance();

                if (client->credit >= 0 && client->balance + client->credit - sum_balance < 0){
                    str = str + " / Credit limit "
                            + string_fmt("%.2f",client->balance + client->credit - sum_balance) + " = "
                            + string_fmt("%.2f",client->balance) + " + "
                            + lexical_cast<string>(client->credit) + " - "
                            + string_fmt("%.2f",sum_balance);
                }

                if (client->limit_d != 0 && client->limit_d - sum_day < 0)
                {
                    str = str + " / Daily limit "
                            + string_fmt("%.2f",client->limit_d - sum_day) + " = "
                            + lexical_cast<string>(client->limit_d) + " - "
                            + string_fmt("%.2f",sum_day);
                }

                if (client->limit_m != 0 && client->limit_m - sum_month < 0)
                {
                    str = str + " / Monthly limit "
                            + string_fmt("%.2f",client->limit_m - sum_month) + " = "
                            + lexical_cast<string>(client->limit_m) + " - "
                            + string_fmt("%.2f",sum_month);
                }

                if (client->disabled)
                {
                    str = str + " / Voip disabled ";
                }

            }
            loader->counter_rwlock.unlock();
        }
    }

    Log::wr(str);
}

void BlackList::log_unlock_phone(string &phone)
{
    string str = "UNLOCK " + phone;

    DataLoader *loader = DataLoader::instance();
    loader->rwlock.lock();
    shared_ptr<UsageObjList> usages_ptr = loader->usage.get(get_tday());
    loader->rwlock.unlock();

    UsageObjList *usages = usages_ptr.get();
    if (usages != 0)
    {
        pUsageObj usage = usages->find(phone.c_str());
        if (usage != 0)
        {
            str = str + " / " + lexical_cast<string>(usage->client_id);
        }
    }

    Log::wr(str);
}
