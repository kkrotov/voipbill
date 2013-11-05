#include "ClientCounter.h"

size_t ClientCounter::item_size() {
    return 0;
}

string ClientCounter::sql(BDb * db) {
    return "";
}

void ClientCounter::parse_item(BDbResult &row, void * obj) {
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

    BDbResult res = db->query(
            "   select r_client_id, m_date, m_sum, d_date, d_sum, a_sum, disabled, disabled_local  " \
                "   from calls.get_amounts() ");
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


        cc.amount_month = parseDate(res.get(1));
        cc.sum_month = res.get_i(2);

        cc.amount_day = parseDate(res.get(3));
        cc.sum_day = res.get_i(4);

        cc.sum = res.get_i(5);

        cc.disabled_global = res.get_b(6);
        cc.disabled_local = res.get_b(7);

        cc.updated = 1;
        counter[res.get_i(0)] = cc;
    }
    db->exec("UPDATE billing.clients set sync=0 where sync > 0");

    loaded = true;

    t.stop();
}

void ClientCounter::reload(BDb * db) {

    t.start();

    bool was_update = false;

    BDbResult res = db->query(
            "   select r_client_id, a_sum  " \
                "   from calls.reget_amounts() ");
    loadtime = time(NULL);
    while (res.next()) {
        ClientCounterObj &cc = this->get(res.get_i(0));
        cc.sum = res.get_i(1);
        cc.updated = 1;
        if (cc.client_id == 0) cc.client_id = res.get_i(0);
        was_update = true;
    }
    if (was_update)
        db->exec("UPDATE billing.clients set sync=0 where sync=2");

    t.stop();
}

