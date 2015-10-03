#include "StatsAccountManager.h"

StatsAccountManager::StatsAccountManager() {
    realtimeStatsAccountParts.push_back(map<int, StatsAccount>());
}

void StatsAccountManager::load(BDb * db) {

    BDbResult res = db->query(
            "   select account_id, extract(epoch from amount_month), sum_month, extract(epoch from amount_day), sum_day, extract(epoch from amount_date), sum, min_call_id, max_call_id " \
            "   from billing.stats_account"
    );

    while (res.next()) {
        StatsAccount &stats = statsAccount[res.get_i(0)];
        stats.account_id = res.get_i(0);
        stats.amount_month = res.get_ll(1);
        stats.sum_month = res.get_d(2);
        stats.amount_day = res.get_ll(3);
        stats.sum_day = res.get_d(4);
        stats.amount_date = res.get_ll(5);
        stats.sum = res.get_d(6);
        stats.min_call_id = res.get_ll(7);
        stats.max_call_id = res.get_ll(8);
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

    // !!! lock_guard<Spinlock> guard(lock);

    while (res.next()) {
        auto iCc = statsAccount.find(res.get_i(0));
        if (iCc != statsAccount.end() || abs(res.get_d(1)) > 0.00001) {
            StatsAccount &stats = statsAccount[res.get_i(0)];
            stats.account_id = res.get_i(0);
            stats.sum = res.get_d(1);
            stats.amount_date = parseDateTime(res.get(2));
        }
    }
}

size_t StatsAccountManager::size() {
    return statsAccount.size();
}

void StatsAccountManager::prepareSaveQuery(stringstream &query) {

    if (realtimeStatsAccountParts[0].size() == 0) {
        return;
    }

    query << "INSERT INTO billing.stats_account(account_id, amount_month, sum_month, amount_day, sum_day, amount_date, sum, min_call_id, max_call_id) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsAccountParts[0]) {
        StatsAccount &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << stats.account_id << "',";
        query << "'" << string_time(stats.amount_month) << "',";
        query << "'" << stats.sum_month << "',";
        query << "'" << string_time(stats.amount_day) << "',";
        query << "'" << stats.sum_day << "',";
        query << "'" << string_time(stats.amount_date) << "',";
        query << "'" << stats.sum << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }

}

void StatsAccountManager::executeSaveQuery(BDb * dbCalls, stringstream &query) {
    string q = query.str();
    if (q.size() > 0) {

        dbCalls->exec(q);

    }
}

void StatsAccountManager::createNewPartition() {
    realtimeStatsAccountParts.push_back(map<int, StatsAccount>());
}

void StatsAccountManager::removePartitionAfterSave() {
    realtimeStatsAccountParts.erase(realtimeStatsAccountParts.begin());
}

void StatsAccountManager::add(CallInfo *callInfo) {

    Call *call = callInfo->call;

    if (call->account_id > 0 && abs(call->cost) < 0.000001) {
        return;
    }


    StatsAccount &stats = statsAccount[call->account_id];

    if (stats.account_id == 0) {
        stats.account_id = call->account_id;
        stats.min_call_id = call->id;
    }

    stats.max_call_id = call->id;

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

    size_t parts = realtimeStatsAccountParts.size();
    map<int, StatsAccount> &realtimeStatsAccount = realtimeStatsAccountParts.at(parts - 1);
    StatsAccount &stats2 = realtimeStatsAccount[call->account_id];
    memcpy(&stats2, &stats, sizeof(StatsAccount));

    forSync.insert(call->account_id);
}

double StatsAccountManager::getSumMonth(int account_id, double vat_rate) {
    auto it1 = statsAccount.find(account_id);
    if (it1 != statsAccount.end()) {
        return it1->second.sumMonth(vat_rate);
    } else {
        return 0;
    }
}

double StatsAccountManager::getSumDay(int account_id, double vat_rate) {
    auto it1 = statsAccount.find(account_id);
    if (it1 != statsAccount.end()) {
        return it1->second.sumDay(vat_rate);
    } else {
        return 0;
    }
}

double StatsAccountManager::getSumBalance(int account_id, double vat_rate) {
    auto it1 = statsAccount.find(account_id);
    if (it1 != statsAccount.end()) {
        return it1->second.sumBalance(vat_rate);
    } else {
        return 0;
    }
}

void StatsAccountManager::getChanges(map<int, StatsAccount> &changes) {
    for (int id : forSync) {
        changes[id] = statsAccount[id];
    }
    forSync.clear();
}

void StatsAccountManager::addChanges(map<int, StatsAccount> &changes) {
    for (auto it : changes) {
        forSync.insert(it.first);
    }
}