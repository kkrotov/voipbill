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

    loaded = true;
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

void StatsAccountManager::save(BDb * dbCalls) {

    if (tmpStatsAccount.size() == 0) {
        return;
    }

    stringstream q;
    q << "INSERT INTO billing.stats_account(account_id, amount_month, sum_month, amount_day, sum_day, amount_date, sum) VALUES\n";
    int i = 0;
    for (auto it : tmpStatsAccount) {
        StatsAccount &stats = it.second;
        if (i > 0) q << ",\n";
        q << "(";
        q << "'" << stats.account_id << "',";
        q << "'" << string_time(stats.amount_month) << "',";
        q << "'" << stats.sum_month << "',";
        q << "'" << string_time(stats.amount_day) << "',";
        q << "'" << stats.sum_day << "',";
        q << "'" << string_time(stats.amount_date) << "',";
        q << "'" << stats.sum << "')";
        i++;
    }

    dbCalls->exec(q.str());
}

void StatsAccountManager::moveRealtimeToTemp() {
    lock_guard<Spinlock> guard(lock);

    move(realtimeStatsAccount, tmpStatsAccount);
}

void StatsAccountManager::moveTempToStored() {
    lock_guard<Spinlock> guard(lock);

    move(tmpStatsAccount, realtimeStatsAccount);
}

void StatsAccountManager::add(CallInfo *callInfo) {

    Call * call = callInfo->call;

    if (call->account_id > 0 && abs(call->cost) < 0.000001) {
        return;
    }

    StatsAccount &stats = realtimeStatsAccount[call->account_id];

    stats.account_id = call->account_id;

    if (abs(call->dt.month - stats.amount_month) < 43200) {
        stats.amount_month = call->dt.month;
        stats.sum_month += call->cost;
    } else if (call->dt.month > stats.amount_month) {
        stats.amount_month = call->dt.month;
        stats.sum_month = call->cost;
    }

    if (abs(call->dt.day - stats.amount_day) < 43200) {
        stats.amount_day = call->dt.day;
        stats.sum_day += call->cost;
    } else if (call->dt.day > stats.amount_day) {
        stats.amount_day = call->dt.day;
        stats.sum_day = call->cost;
    }

    if (callInfo->account != nullptr && call->connect_time >= callInfo->account->amount_date) {
        stats.sum += call->cost;
    }
}

double StatsAccountManager::getSumMonth(int account_id, double vat_rate) {
    double sum = 0;

    auto it1 = storedStatsAccount.find(account_id);
    if (it1 != storedStatsAccount.end()) {
        sum += it1->second.sumMonth(vat_rate);
    }

    auto it2 = realtimeStatsAccount.find(account_id);
    if (it2 != realtimeStatsAccount.end()) {
        sum += it2->second.sumMonth(vat_rate);
    }

    auto it3 = tmpStatsAccount.find(account_id);
    if (it3 != tmpStatsAccount.end()) {
        sum += it3->second.sumMonth(vat_rate);
    }

    return sum;
}

double StatsAccountManager::getSumDay(int account_id, double vat_rate) {
    double sum = 0;

    auto it1 = storedStatsAccount.find(account_id);
    if (it1 != storedStatsAccount.end()) {
        sum += it1->second.sumDay(vat_rate);
    }

    auto it2 = realtimeStatsAccount.find(account_id);
    if (it2 != realtimeStatsAccount.end()) {
        sum += it2->second.sumDay(vat_rate);
    }

    auto it3 = tmpStatsAccount.find(account_id);
    if (it3 != tmpStatsAccount.end()) {
        sum += it3->second.sumDay(vat_rate);
    }

    return sum;
}

double StatsAccountManager::getSumBalance(int account_id, double vat_rate) {
    double sum = 0;

    auto it1 = storedStatsAccount.find(account_id);
    if (it1 != storedStatsAccount.end()) {
        sum += it1->second.sumBalance(vat_rate);
    }

    auto it2 = realtimeStatsAccount.find(account_id);
    if (it2 != realtimeStatsAccount.end()) {
        sum += it2->second.sumBalance(vat_rate);
    }

    auto it3 = tmpStatsAccount.find(account_id);
    if (it3 != tmpStatsAccount.end()) {
        sum += it3->second.sumBalance(vat_rate);
    }

    return sum;
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