#include "BlackList.h"

#include "UdpControlClient.h"
#include "../../src/classes/Log.h"
#include "../../src/common.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

bool BlackList::fetch() {
    vector<string> curr_list;
    if (!udp_blacklist(curr_list)) {
        //Log::error("Cant fetch black list from opanca");
        return false;
    }

    lock_guard<Spinlock> guard(lock);

    {
        blacklist.clear();
        for(string &phone : curr_list) {
            blacklist.insert(phone);
        }
    }

    return true;
}

void BlackList::push(set<string> &wanted_blacklist) {

    vector<string> list;

    {
        lock_guard<Spinlock> guard(lock);

        list_to_add.clear();
        list_to_del.clear();

        for (auto phone : blacklist) {
            auto it = wanted_blacklist.find(phone);
            if (it == wanted_blacklist.end()) {
                list_to_del.insert(phone);
            }
        }

        for (auto phone : wanted_blacklist) {
            auto it = blacklist.find(phone);
            if (it == blacklist.end()) {
                list_to_add.insert(phone);
            }
        }
    }

    for (auto phone : list_to_add) {
        if (!udp_lock(phone)) {
            Log::error("Cant lock phone " + phone);
            continue;
        }

        {
            lock_guard<Spinlock> guard(lock);
            blacklist.insert(phone);
        }

        log_lock_phone(phone);
    }

    for (auto phone : list_to_del) {
        if (!udp_unlock(phone)) {
            Log::error("Cant unlock phone " + phone);
            continue;
        }

        {
            lock_guard<Spinlock> guard(lock);
            blacklist.erase(phone);
        }

        log_unlock_phone(phone);
    }
}

void BlackList::log_lock_phone(const string &phone) {
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

                double tax_rate = 0;
                auto organization = preparedData.organization->find(client->organization_id, time(nullptr));
                if (organization != nullptr) {
                    tax_rate = organization->tax_rate / 100.0;
                }

                ClientCounterObj clientCounter = DataBillingContainer::instance()->clientCounter.get()->get(client->id);
                double sum_month = clientCounter.sumMonth(tax_rate);
                double sum_day = clientCounter.sumDay(tax_rate);
                double sum_balance = clientCounter.sumBalance(tax_rate);

                if (client->isConsumedCreditLimit(sum_balance)) {
                    str = str + " / Credit limit "
                            + string_fmt("%.2f", client->balance + client->credit + sum_balance) + " = "
                            + string_fmt("%.2f", client->balance) + " + "
                            + lexical_cast<string>(client->credit) + " + "
                            + string_fmt("%.2f", sum_balance);
                }

                if (client->isConsumedDailyLimit(sum_day)) {
                    str = str + " / Daily limit "
                            + string_fmt("%.2f", client->limit_d + sum_day) + " = "
                            + lexical_cast<string>(client->limit_d) + " + "
                            + string_fmt("%.2f", sum_day);
                }

                if (client->isConsumedMonthlyLimit(sum_month)) {
                    str = str + " / Monthly limit "
                            + string_fmt("%.2f", client->limit_m + sum_month) + " = "
                            + lexical_cast<string>(client->limit_m) + " + "
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

void BlackList::log_unlock_phone(const string &phone) {
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
