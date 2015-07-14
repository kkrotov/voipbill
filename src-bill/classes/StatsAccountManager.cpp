#include "StatsAccountManager.h"

void StatsAccountManager::load(BDb * db) {

    /*
    time_t d_date = get_tday(time(nullptr));
    time_t m_date = get_tmonth(time(nullptr));

    string sDay = string_date(d_date);

    string sPrevMonth = string_date(m_date - 32 * 86400);

    BDbResult res = db->query(
            "   select " \
            "       c.id, " \
            "       COALESCE(d.d_sum, 0), " \
            "       COALESCE(a.a_sum, 0), " \
            "       c.amount_date " \
            "   from billing.clients c  " \
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
            "           COALESCE(d.d_sum, 0) != 0 OR " \
            "           COALESCE(a.a_sum, 0) != 0 "  \
            "       ) ");

    db->exec("UPDATE billing.clients set sync=0 where sync > 0");
    */

    BDbResult res = db->query(
            "   select account_id, extract(epoch from amount_day), sum_day, extract(epoch from amount_date), sum " \
            "   from billing.stats_account"
    );

    while (res.next()) {
        StatsAccount &stats = storedStatsAccount[res.get_i(0)];
        stats.account_id = res.get_i(0);
        stats.amount_day = res.get_ll(1);
        stats.sum_day = res.get_d(2);
        stats.amount_date = res.get_ll(3);
        stats.sum = res.get_d(4);
    }
}


void StatsAccountManager::reload(BDb * db) {

    time_t m_date = get_tmonth(time(nullptr));
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
        auto iCc = storedStatsAccount.find(res.get_i(0));
        if (iCc != storedStatsAccount.end() || abs(res.get_d(1)) > 0.00001) {
            StatsAccount &stats = storedStatsAccount[res.get_i(0)];
            stats.account_id = res.get_i(0);
            stats.sum = res.get_d(1);
            stats.amount_date = parseDateTime(res.get(2));
        }
    }
}

size_t StatsAccountManager::size() {
    return storedStatsAccount.size() + realtimeStatsAccount.size() + tmpStatsAccount.size();
}

void StatsAccountManager::moveRealtimeToTemp() {
    lock_guard<Spinlock> guard(lock);

    move(realtimeStatsAccount, tmpStatsAccount);
}

void StatsAccountManager::moveTempToStored() {
    lock_guard<Spinlock> guard(lock);

    move(tmpStatsAccount, realtimeStatsAccount);
}

void StatsAccountManager::move(map<int, StatsAccount> &fromStatsAccount, map<int, StatsAccount> &toStatsAccount) {
    for (pair<const int, StatsAccount> &fromStats : fromStatsAccount) {
        StatsAccount &toStats = toStatsAccount[fromStats.first];
        if (toStats.account_id == 0) {
            toStats.account_id = fromStats.second.account_id;
            toStats.sum = fromStats.second.sum;
            toStats.sum_day = fromStats.second.sum_day;
            toStats.amount_day = fromStats.second.amount_day;
            toStats.amount_date = fromStats.second.amount_date;
        } else {
            if (toStats.amount_day != fromStats.second.amount_day) {
                toStats.sum_day = fromStats.second.sum_day;
            } else {
                toStats.sum_day += fromStats.second.sum_day;
            }
            if (toStats.amount_date != fromStats.second.amount_date) {
                toStats.sum = fromStats.second.sum;
            } else {
                toStats.sum += fromStats.second.sum;
            }
        }
    }

    fromStatsAccount.clear();
}