#include "ClientCounter.h"

size_t ClientCounter::item_size() {
    return 0;
}

string ClientCounter::sql(BDb * db) {
    return "";
}

inline void ClientCounter::parse_item(BDbResult &row, void * obj) {
    *(int*) obj = row.size();
}

ClientCounterObj &ClientCounter::get(const int client_id) {
    return counter[client_id];
}

void ClientCounter::set(int client_id, ClientCounterObj &new_value) {
    counter[client_id] = new_value;
}

void ClientCounter::clear() {
    counter.clear();
}

void ClientCounter::append(ClientCounter *newcl) {
    map<int, ClientCounterObj>::iterator i;
    for (i = newcl->counter.begin(); i != newcl->counter.end(); ++i) {
        int key = i->first;
        ClientCounterObj &add_value = i->second;

        ClientCounterObj &value = counter[key];

        if (value.amount_month < add_value.amount_month) {
            value.amount_month = add_value.amount_month;
            value.sum_month = add_value.sum_month;
        } else if (value.amount_month == add_value.amount_month) {
            value.sum_month += add_value.sum_month;
        }

        if (value.amount_day < add_value.amount_day) {
            value.amount_day = add_value.amount_day;
            value.sum_day = add_value.sum_day;
        } else if (value.amount_day == add_value.amount_day) {
            value.sum_day += add_value.sum_day;
        }

        value.sum += add_value.sum;

        value.updated = 1;

        if (value.client_id == 0) value.client_id = key;
    }
}

void ClientCounter::load(BDb * db, time_t dt) {

    t.start();

    this->dt = dt;
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
            "           (c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum::numeric * 1.18 / 100::numeric,0)) or " \
            "           (c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum::numeric * 1.18 / 100::numeric,0)) or " \
            "           (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) or " \
            "           c.voip_disabled as boolean " \
            "       ), " \
            "       cast( " \
            "           (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) " \
            "           and coalesce(c.last_payed_month < '" + sMonth + "'::date, true) as boolean " \
            "       ) " \
            "   from billing.clients c  " \
            "   left join " \
            "       (   select client_id, sum(amount) m_sum from calls.calls c " \
            "           where " \
            "               c.time >= '" + sMonth + "'::date and " \
            "               c.time < '" + sMonth + "'::date + '1 month'::interval " \
            "           group by client_id " \
            "       ) as m " \
            "   on c.id = m.client_id " \
            "   left join " \
            "       (   select client_id, sum(amount) d_sum from calls.calls c " \
            "           where " \
            "           c.time >= '" + sDay + "'::date and " \
            "           c.time < '" + sDay + "'::date + '1 day'::interval " \
            "           group by client_id " \
            "       ) as d " \
            "   on c.id = d.client_id " \
            "   left join " \
            "       (   select cl.id client_id, sum(amount) a_sum " \
            "           from calls.calls c " \
            "           left join billing.clients cl " \
            "           on c.client_id=cl.id " \
            "           where " \
            "               c.time >= '" + sPrevMonth + "' " \
            "               and c.time >= cl.amount_date " \
            "           group by cl.id " \
            "       ) as a " \
            "   on c.id = a.client_id " \
            "   where " \
            "       ( " \
            "           COALESCE(m.m_sum, 0) != 0 OR " \
            "           COALESCE(d.d_sum, 0) != 0 OR " \
            "           COALESCE(a.a_sum, 0) != 0 OR "  \
            "           cast( "  \
            "               (c.voip_limit_day != 0 and c.voip_limit_day < coalesce(d.d_sum::numeric * 1.18 / 100::numeric,0)) or "  \
            "               (c.voip_limit_month != 0 and c.voip_limit_month < coalesce(m.m_sum::numeric * 1.18 / 100::numeric,0)) or "  \
            "               (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) or "  \
            "               c.voip_disabled as boolean "  \
            "           ) OR "  \
            "           cast( "  \
            "               (c.credit >= 0 and c.balance + c.credit < coalesce(a.a_sum::numeric * 1.18 / 100::numeric,0)) "  \
            "               and coalesce(c.last_payed_month < '" + sMonth + "'::date, true) as boolean "  \
            "           ) " \
            "       ) ");

    db->exec("UPDATE billing.clients set sync=0 where sync > 0");

    loadtime = time(NULL);
    last_use = loadtime;
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
        cc.sum_month = res.get_i(1);

        cc.amount_day = d_date;
        cc.sum_day = res.get_i(2);

        cc.sum = res.get_i(3);

        cc.disabled_global = res.get_b(4);
        cc.disabled_local = res.get_b(5);

        cc.updated = 1;
        counter[cc.client_id] = cc;
    }

    loaded = true;

    t.stop();
}

void ClientCounter::reload(BDb * db) {

    t.start();

    time_t m_date = get_tmonth();
    string sPrevMonth = string_date(m_date - 32 * 86400);

    db->exec("UPDATE billing.clients set sync=2 where sync=1");

    BDbResult res = db->query(
            "   select c.id, COALESCE(a.a_sum,0) " \
            "   from billing.clients c " \
            "   left join " \
            "       (   select cl.id client_id, sum(amount) a_sum " \
            "           from calls.calls c " \
            "           left join billing.clients cl on c.client_id=cl.id " \
            "           where " \
            "               c.time >= '" + sPrevMonth + "'::date " \
            "               and c.time >= cl.amount_date " \
            "               and cl.sync = 2 " \
            "           group by cl.id " \
            "       ) as a " \
            "   on c.id = a.client_id " \
            "   where c.sync = 2 ");

    db->exec("UPDATE billing.clients set sync=0 where sync=2");

    loadtime = time(NULL);
    while (res.next()) {
        ClientCounterObj &cc = this->get(res.get_i(0));
        cc.client_id = res.get_i(0);
        cc.sum = res.get_i(1);
        cc.updated = 1;
    }

    t.stop();
}

