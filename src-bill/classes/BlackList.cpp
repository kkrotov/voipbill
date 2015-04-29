#include "BlackList.h"

#include "UdpControlClient.h"
#include "../../src/classes/Log.h"
#include "../../src/common.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

BlackList::BlackList() {
}

void BlackList::add(const string &phone) {
    lock_guard<mutex> lk(rwlock);
    if (blacklist.find(phone) == blacklist.end()) {
        list_to_add[phone] = true;
    }
    if (list_to_del.find(phone) != list_to_del.end()) {
        list_to_del.erase(phone);
    }
}

void BlackList::del(const string &phone) {
    lock_guard<mutex> lk(rwlock);
    if (blacklist.find(phone) != blacklist.end()) {
        list_to_del[phone] = true;
    }
    if (list_to_add.find(phone) != list_to_add.end()) {
        list_to_add.erase(phone);
    }
}

void BlackList::change_lock(const string &phone, bool need_lock) {
    if (is_locked(phone) != need_lock) {
        if (need_lock) {
            add(phone);
        } else {
            del(phone);
        }
    }
}

void BlackList::lock(const string &phone) {
    if (!is_locked(phone)) {
        add(phone);
    }
}

void BlackList::unlock(const string &phone) {
    if (is_locked(phone)) {
        del(phone);
    }
}

bool BlackList::fetch() {
    vector<string> curr_list;
    if (udp_blacklist(curr_list) == false) {
        //Log::error("Can nostring fetch black lisstring from opanca");
        return false;
    }

    time_t current_time = time(NULL);

    lock_guard<mutex> lk(rwlock);

    {
        vector<string>::iterator i = curr_list.begin();
        while (i != curr_list.end()) {
            string &phone = *i;
            blacklist[phone] = current_time;
            ++i;
        }
    }

    {
        map<string, time_t>::iterator i = blacklist.begin();
        while (i != blacklist.end()) {
            if (i->second != current_time) {
                blacklist.erase(i++);
            } else
                ++i;
        }
    }


    {
        map<string, bool>::iterator i = list_to_add.begin();
        while (i != list_to_add.end()) {
            if (blacklist.find(i->first) != blacklist.end()) {
                list_to_add.erase(i++);
            } else
                ++i;
        }
    }

    {
        map<string, bool>::iterator i = list_to_del.begin();
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
        vector<string> list;

        {
            lock_guard<mutex> lk(rwlock);
            map<string, bool>::iterator i = list_to_add.begin();
            while (i != list_to_add.end()) {
                list.push_back(i->first);
                ++i;
            }
        }

        auto ii = list.begin();
        while (ii != list.end()) {
            string &phone = *ii;
            if (udp_lock(phone) == false) {
                Log::error("Can nostring lock phone " + phone);
                ++ii;
                continue;
            }

            {
                lock_guard<mutex> lk(rwlock);
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
        vector<string> list;

        {
            lock_guard<mutex> lk(rwlock);
            map<string, bool>::iterator i = list_to_del.begin();
            while (i != list_to_del.end()) {
                list.push_back(i->first);
                ++i;
            }
        }

        auto ii = list.begin();
        while (ii != list.end()) {
            string &phone = *ii;
            if (!udp_unlock(phone)) {
                Log::error("Can nostring unlock phone " + phone);
                ++ii;
                continue;
            }

            {
                lock_guard<mutex> lk(rwlock);
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

bool BlackList::is_locked(const string &phone) {
    if (blacklist.find(phone) != blacklist.end()) {
        return !(list_to_del.find(phone) != list_to_del.end());
    } else {
        return list_to_add.find(phone) != list_to_add.end();
    }
}

void BlackList::log_lock_phone(string &phone) {
    string str = "LOCK " + phone;

    PreparedData preparedData;
    if (DataContainer::instance()->prepareData(preparedData, time(nullptr))) {
        auto serviceNumber = preparedData.serviceNumber->find(atoll(phone.c_str()), time(nullptr));
        Client * client;
        if (serviceNumber != nullptr) {
            str = str + " / " + lexical_cast<string>(serviceNumber->client_account_id);
            client = preparedData.client->find(serviceNumber->client_account_id);
        } else {
            client = nullptr;
        }

        if (client != nullptr) {

            if (DataBillingContainer::instance()->ready()) {
                ClientCounterObj &clientCounter = DataBillingContainer::instance()->clientCounter.get()->get(client->id);
                double sum_month = clientCounter.sumMonth();
                double sum_day = clientCounter.sumDay();
                double sum_balance = clientCounter.sumBalance();

                if (client->isConsumedCreditLimit(sum_balance)) {
                    str = str + " / Credit limit "
                            + string_fmt("%.2f", client->balance + client->credit - sum_balance) + " = "
                            + string_fmt("%.2f", client->balance) + " + "
                            + lexical_cast<string>(client->credit) + " - "
                            + string_fmt("%.2f", sum_balance);
                }

                if (client->isConsumedDailyLimit(sum_day)) {
                    str = str + " / Daily limit "
                            + string_fmt("%.2f", client->limit_d - sum_day) + " = "
                            + lexical_cast<string>(client->limit_d) + " - "
                            + string_fmt("%.2f", sum_day);
                }

                if (client->isConsumedMonthlyLimit(sum_month)) {
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

    PreparedData preparedData;
    if (DataContainer::instance()->prepareData(preparedData, time(nullptr))) {

        auto serviceNumber = preparedData.serviceNumber->find(atoll(phone.c_str()), time(nullptr));
        if (serviceNumber != 0) {
            str = str + " / " + lexical_cast<string>(serviceNumber->client_account_id);
        }
    }

    Log::notice(str);
}
