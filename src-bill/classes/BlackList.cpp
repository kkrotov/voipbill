#include "BlackList.h"

#include "UdpControlClient.h"
#include "../../src/classes/Log.h"
#include "DataLoader.h"
#include "../../src/common.h"

BlackList::BlackList() {
}

void BlackList::add(long long int phone) {
    lock_guard<mutex> lk(lock);
    if (blacklist.find(phone) == blacklist.end()) {
        list_to_add[phone] = true;
    }
    if (list_to_del.find(phone) != list_to_del.end()) {
        list_to_del.erase(phone);
    }
}

void BlackList::del(long long int phone) {
    lock_guard<mutex> lk(lock);
    if (blacklist.find(phone) != blacklist.end()) {
        list_to_del[phone] = true;
    }
    if (list_to_add.find(phone) != list_to_add.end()) {
        list_to_add.erase(phone);
    }
}

bool BlackList::fetch() {
    vector<string> curr_list;
    if (udp_blacklist(curr_list) == false) {
        Log::error("Can not fetch black list from opanca");
        return false;
    }

    time_t current_time = time(NULL);

    lock_guard<mutex> lk(lock);

    {
        vector<string>::iterator i = curr_list.begin();
        while (i != curr_list.end()) {
            long long int phone = atoll(((string) * i).c_str());
            blacklist[phone] = current_time;
            ++i;
        }
    }

    {
        map<long long int, time_t>::iterator i = blacklist.begin();
        while (i != blacklist.end()) {
            if (i->second != current_time) {
                blacklist.erase(i++);
            } else
                ++i;
        }
    }


    {
        map<long long int, bool>::iterator i = list_to_add.begin();
        while (i != list_to_add.end()) {
            if (blacklist.find(i->first) != blacklist.end()) {
                list_to_add.erase(i++);
            } else
                ++i;
        }
    }

    {
        map<long long int, bool>::iterator i = list_to_del.begin();
        while (i != list_to_del.end()) {
            if (blacklist.find(i->first) == blacklist.end()) {
                list_to_del.erase(i++);
            } else
                ++i;
        }
    }

    return true;
}

void BlackList::push() {
    {
        vector<long long int> list;

        {
            lock_guard<mutex> lk(lock);
            map<long long int, bool>::iterator i = list_to_add.begin();
            while (i != list_to_add.end()) {
                list.push_back(i->first);
                ++i;
            }
        }

        vector<long long int>::iterator ii = list.begin();
        while (ii != list.end()) {
            string phone = lexical_cast<string>(*ii);
            if (udp_lock(phone) == false) {
                Log::error("Can not lock phone " + phone);
                ++ii;
                continue;
            }

            {
                lock_guard<mutex> lk(lock);
                if (blacklist.find(*ii) == blacklist.end()) {
                    blacklist[*ii] = time(NULL);
                }
                if (list_to_add.find(*ii) != list_to_add.end()) {
                    list_to_add.erase(*ii);
                }
            }

            log_lock_phone(phone);

            ++ii;
        }
    }

    {
        vector<long long int> list;

        {
            lock_guard<mutex> lk(lock);
            map<long long int, bool>::iterator i = list_to_del.begin();
            while (i != list_to_del.end()) {
                list.push_back(i->first);
                ++i;
            }
        }

        vector<long long int>::iterator ii = list.begin();
        while (ii != list.end()) {
            string phone = lexical_cast<string>(*ii);
            if (udp_unlock(phone) == false) {
                Log::error("Can not unlock phone " + phone);
                ++ii;
                continue;
            }

            {
                lock_guard<mutex> lk(lock);
                if (blacklist.find(*ii) != blacklist.end()) {
                    blacklist.erase(*ii);
                }
                if (list_to_del.find(*ii) != list_to_del.end()) {
                    list_to_del.erase(*ii);
                }
            }

            log_unlock_phone(phone);

            ++ii;
        }
    }

}

bool BlackList::is_locked(long long int phone) {
    if (blacklist.find(phone) != blacklist.end()) {
        if (list_to_del.find(phone) != list_to_del.end()) {
            return false;
        } else {
            return true;
        }
    } else {
        if (list_to_add.find(phone) != list_to_add.end()) {
            return true;
        } else {
            return false;
        }
    }
}

void BlackList::log_lock_phone(string &phone) {
    string str = "LOCK " + phone;

    DataLoader *loader = DataLoader::instance();
    UsageObjList *usages;
    ClientObjList *clients;

    {
        lock_guard<mutex> lk(loader->rwlock);
        shared_ptr<UsageObjList> usages_ptr = loader->usage.get(get_tday());
        shared_ptr<ClientObjList> clients_ptr = loader->client;

        usages = usages_ptr.get();
        clients = clients_ptr.get();
    }


    if (usages != 0 && clients != 0) {
        pUsageObj usage = usages->find(phone.c_str());
        pClientObj client;
        if (usage != 0) {
            str = str + " / " + lexical_cast<string>(usage->client_id);
            client = clients->find(usage->client_id);
        } else {
            client = 0;
        }

        if (client != 0) {
            lock_guard<mutex> lk(loader->counter_rwlock);

            shared_ptr<ClientCounter> clients_counters_ptr = loader->counter_client;
            ClientCounter *clients_counters = clients_counters_ptr.get();
            if (clients_counters != 0) {
                ClientCounterObj &client_counter = clients_counters->get(usage->client_id);
                double sum_month = client_counter.sumMonth();
                double sum_day = client_counter.sumDay();
                double sum_balance = client_counter.sumBalance();

                if (client->credit >= 0 && client->balance + client->credit - sum_balance < 0) {
                    str = str + " / Credit limit "
                            + string_fmt("%.2f", client->balance + client->credit - sum_balance) + " = "
                            + string_fmt("%.2f", client->balance) + " + "
                            + lexical_cast<string>(client->credit) + " - "
                            + string_fmt("%.2f", sum_balance);
                }

                if (client->limit_d != 0 && client->limit_d - sum_day < 0) {
                    str = str + " / Daily limit "
                            + string_fmt("%.2f", client->limit_d - sum_day) + " = "
                            + lexical_cast<string>(client->limit_d) + " - "
                            + string_fmt("%.2f", sum_day);
                }

                if (client->limit_m != 0 && client->limit_m - sum_month < 0) {
                    str = str + " / Monthly limit "
                            + string_fmt("%.2f", client->limit_m - sum_month) + " = "
                            + lexical_cast<string>(client->limit_m) + " - "
                            + string_fmt("%.2f", sum_month);
                }

                if (client->disabled) {
                    str = str + " / Voip disabled ";
                }

            }
        }
    }

    Log::notice(str);
}

void BlackList::log_unlock_phone(string &phone) {
    string str = "UNLOCK " + phone;

    DataLoader *loader = DataLoader::instance();
    UsageObjList * usages;

    {
        lock_guard<mutex> lk(loader->rwlock);
        shared_ptr<UsageObjList> usages_ptr = loader->usage.get(get_tday());
        usages = usages_ptr.get();
    }


    if (usages != 0) {
        pUsageObj usage = usages->find(phone.c_str());
        if (usage != 0) {
            str = str + " / " + lexical_cast<string>(usage->client_id);
        }
    }

    Log::notice(str);
}
