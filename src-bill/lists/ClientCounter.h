#pragma once

#include "../../src/lists/ObjList.h"
#include "../../src/classes/Spinlock.h"
#include "../models/ClientCounterObj.h"
#include "../models/Client.h"
#include "../models/Call.h"
#include <mutex>
#include <map>

using namespace std;

class ClientCounter : public ObjList<ClientCounterObj> {
protected:
    string sql(BDb * db) {
        return "";
    }
    inline void parse_item(BDbResult &row, ClientCounterObj * item) {
        item->client_id = 0;
    }

public:
    Spinlock lock;

    unsigned long long int marker = 0;

    bool needTotalSync = true;

    map<int, ClientCounterObj> counter;
    map<int, unsigned long long int> changes;

    ClientCounterObj get(int client_id) {
        lock_guard<Spinlock> guard(lock);
        return counter[client_id];
    }

    void add(Call * call, Client * client) {

        if (abs(call->cost) < 0.000001) {
            return;
        }

        lock_guard<Spinlock> guard(lock);

        ClientCounterObj &value = counter[call->account_id];

        if (value.client_id == 0) {
            value.client_id = call->account_id;
        }

        if (abs(call->dt.month - value.amount_month) < 43200) {
            value.amount_month = call->dt.month;
            value.sum_month += call->cost;
        } else if (call->dt.month > value.amount_month) {
            value.amount_month = call->dt.month;
            value.sum_month = call->cost;
        }

        if (abs(call->dt.day - value.amount_day) < 43200) {
            value.amount_day = call->dt.day;
            value.sum_day += call->cost;
        } else if (call->dt.day > value.amount_day) {
            value.amount_day = call->dt.day;
            value.sum_day = call->cost;
        }

        if (client != nullptr && call->connect_time >= client->amount_date) {
            value.sum += call->cost;
        }

        marker++;
        changes[call->account_id] = marker;
    }

    void getClients(vector<ClientCounterObj> &destClients) {
        lock_guard<Spinlock> guard(lock);

        destClients.reserve(counter.size());
        for (auto pair : counter) {
            destClients.push_back(pair.second);
        }
    }

    void getChanges(unsigned long long int &destMarker, vector<int> &destChanges, bool &destNeedTotalSync) {
        lock_guard<Spinlock> guard(lock);

        destMarker = marker;
        destNeedTotalSync = needTotalSync;

        if (needTotalSync) {
            destChanges.reserve(counter.size());
            for (auto pair : counter) {
                destChanges.push_back(pair.first);
            }
        } else {
            destChanges.reserve(changes.size());
            for (auto pair : changes) {
                destChanges.push_back(pair.first);
            }
        }
    }

    void fixChanges(unsigned long long int old_marker) {
        lock_guard<Spinlock> guard(lock);

        auto it = changes.begin();
        while (it != changes.end()) {
            if (it->second <= old_marker) {
                changes.erase(it++);
            } else {
                ++it;
            }
        }
    }

    void load(BDb * db) {

        time_t d_date = get_tday();
        time_t m_date = get_tmonth();

        string sDay = string_date(d_date);
        string sMonth = string_date(m_date);

        string sPrevMonth = string_date(m_date - 32 * 86400);

        BDbResult res = db->query(
            "   select " \
            "       c.id, " \
            "       COALESCE(m.m_sum, 0), " \
            "       COALESCE(d.d_sum, 0), " \
            "       COALESCE(a.a_sum, 0), " \
            "       c.amount_date " \
            "   from billing.clients c  " \
            "   left join " \
            "       (   select account_id, sum(cost) m_sum from calls_raw.calls_raw c " \
            "           where " \
            "               c.connect_time >= '" + sMonth + "'::date and " \
            "               c.connect_time < '" + sMonth + "'::date + '1 month'::interval " \
            "           group by account_id " \
            "       ) as m " \
            "   on c.id = m.account_id " \
            "   left join " \
            "       (   select account_id, sum(cost) d_sum from calls_raw.calls_raw c " \
            "           where " \
            "           c.connect_time >= '" + sDay + "'::date and " \
            "           c.connect_time < '" + sDay + "'::date + '1 day'::interval " \
            "           group by account_id " \
            "       ) as d " \
            "   on c.id = d.account_id " \
            "   left join " \
            "       (   select account_id, sum(cost) a_sum " \
            "           from calls_raw.calls_raw c " \
            "           left join billing.clients cl " \
            "           on c.account_id=cl.id " \
            "           where " \
            "               c.connect_time >= '" + sPrevMonth + "' " \
            "               and (c.connect_time >= cl.amount_date or cl.amount_date is null) " \
            "           group by account_id " \
            "       ) as a " \
            "   on c.id = a.account_id " \
            "   where " \
            "       ( " \
            "           COALESCE(m.m_sum, 0) != 0 OR " \
            "           COALESCE(d.d_sum, 0) != 0 OR " \
            "           COALESCE(a.a_sum, 0) != 0 "  \
            "       ) ");

        db->exec("UPDATE billing.clients set sync=0 where sync > 0");

        loadtime = time(nullptr);
        while (res.next()) {
            struct tm ttt;
            ttt.tm_isdst = 0;
            ttt.tm_wday = 0;
            ttt.tm_yday = 0;
            ttt.tm_hour = 0;
            ttt.tm_min = 0;
            ttt.tm_sec = 0;

            ClientCounterObj cc;

            cc.client_id = res.get_i(0);

            cc.amount_month = m_date;
            cc.sum_month = res.get_d(1);

            cc.amount_day = d_date;
            cc.sum_day = res.get_d(2);

            cc.sum = res.get_d(3);
            cc.amount_date = parseDateTime(res.get(4));

            counter[cc.client_id] = cc;
        }
    }

    void reload(BDb * db) {

        time_t m_date = get_tmonth();
        string sPrevMonth = string_date(m_date - 32 * 86400);

        db->exec("UPDATE billing.clients set sync=2 where sync=1");

        BDbResult res = db->query(
            "   select c.id, COALESCE(a.a_sum,0), c.amount_date " \
            "   from billing.clients c " \
            "   left join " \
            "       (   select account_id, sum(cost) a_sum " \
            "           from calls_raw.calls_raw c " \
            "           left join billing.clients cl on c.account_id=cl.id " \
            "           where " \
            "               c.connect_time >= '" + sPrevMonth + "'::date " \
            "               and (c.connect_time >= cl.amount_date or cl.amount_date is null) " \
            "               and cl.sync = 2 " \
            "           group by account_id " \
            "       ) as a " \
            "   on c.id = a.account_id " \
            "   where c.sync = 2 ");

        db->exec("UPDATE billing.clients set sync=0 where sync=2");

        lock_guard<Spinlock> guard(lock);

        while (res.next()) {
            auto iCc = counter.find(res.get_i(0));
            if (iCc != counter.end() || abs(res.get_d(1)) > 0.00001) {
                ClientCounterObj &cc = counter[res.get_i(0)];
                cc.client_id = res.get_i(0);
                cc.sum = res.get_d(1);
                cc.amount_date = parseDateTime(res.get(2));

                marker++;
                changes[cc.client_id] = marker;
            }
        }

        loadtime = time(nullptr);
    }

    size_t size() {
        return counter.size();
    }
};


