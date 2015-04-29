#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ClientCounterObj.h"
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
    bool needTotalSync = true;

    map<int, ClientCounterObj> counter;

    ClientCounterObj &get(int client_id) {
        return counter[client_id];
    }

    void set(int client_id, ClientCounterObj &new_value) {
        counter[client_id] = new_value;
    }

    void load(BDb * db) {

        counter.clear();

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
            "       cast( " \
            "           (c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum * 1.18,0)) or " \
            "           (c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum * 1.18,0)) or " \
            "           (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum * 1.18,0)) or " \
            "           c.voip_disabled as boolean " \
            "       ), " \
            "       cast( " \
            "           (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum * 1.18,0)) " \
            "           and coalesce(c.last_payed_month < '" + sMonth + "'::date, true) as boolean " \
            "       ) " \
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
            "           COALESCE(a.a_sum, 0) != 0 OR "  \
            "           cast( "  \
            "               (c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum * 1.18,0)) or "  \
            "               (c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum * 1.18,0)) or "  \
            "               (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum * 1.18,0)) or "  \
            "               c.voip_disabled as boolean "  \
            "           ) OR "  \
            "           cast( "  \
            "               (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum * 1.18,0)) "  \
            "               and coalesce(c.last_payed_month < '" + sMonth + "'::date, true) as boolean "  \
            "           ) " \
            "       ) ");

        db->exec("UPDATE billing.clients set sync=0 where sync > 0");

        loadtime = time(NULL);
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

            cc.disabled_global = res.get_b(4);
            cc.disabled_local = res.get_b(5);

            cc.updated = 1;
            counter[cc.client_id] = cc;
        }
    }

    void reload(BDb * db) {

        time_t m_date = get_tmonth();
        string sPrevMonth = string_date(m_date - 32 * 86400);

        db->exec("UPDATE billing.clients set sync=2 where sync=1");

        BDbResult res = db->query(
                "   select c.id, COALESCE(a.a_sum,0) " \
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

        loadtime = time(NULL);
        while (res.next()) {
            ClientCounterObj &cc = this->get(res.get_i(0));
            cc.client_id = res.get_i(0);
            cc.sum = res.get_d(1);
            cc.updated = 1;
        }
    }

    size_t size() {
        return counter.size();
    }
};


