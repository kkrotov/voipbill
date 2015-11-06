#include "StatsAccountManager.h"
#include "Exception.h"
#include "AppBill.h"
#include "../data/DataBillingContainer.h"

StatsAccountManager::StatsAccountManager() {
    realtimeStatsAccountParts.push_back(map<int, StatsAccount>());
}

void StatsAccountManager::load(BDb * db) {
    forSync.clear();
    statsAccount.clear();
    realtimeStatsAccountParts.clear();
    realtimeStatsAccountParts.push_back(map<int, StatsAccount>());

    BDbResult res = db->query(
            "   select account_id, extract(epoch from amount_month), sum_month, extract(epoch from amount_day), sum_day, extract(epoch from amount_date), sum " \
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
    }

    loaded = true;
}

void StatsAccountManager::recalc(BDb * db) {

    time_t d_date = get_tday(time(nullptr), 0);
    time_t m_date = get_tmonth(time(nullptr), 0);

    string sDay = string_date(d_date, 3);
    string sMonth = string_date(m_date, 4);

    string sPrevMonth = string_date(m_date - 32 * 86400, 5);

    db->exec("DELETE FROM billing.stats_account");

    db->exec(
            "   INSERT INTO billing.stats_account(account_id, amount_month, sum_month, amount_day, sum_day, amount_date, sum)"
            "   select " \
            "       c.id, " \
            "       '" + sMonth + "', " \
            "       COALESCE(m.m_sum, 0), " \
            "       '" + sDay + "', " \
            "       COALESCE(d.d_sum, 0), " \
            "       c.amount_date, " \
            "       COALESCE(a.a_sum, 0) " \
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
            "               and c.connect_time >= cl.amount_date " \
            "           group by account_id " \
            "       ) as a " \
            "   on c.id = a.account_id " \
            "   where " \
            "       ( " \
            "           COALESCE(m.m_sum, 0) != 0 OR " \
            "           COALESCE(d.d_sum, 0) != 0 OR " \
            "           COALESCE(a.a_sum, 0) != 0 "  \
            "       ) ");

    needClear = true;
}

void StatsAccountManager::reloadSum(BDb * db, list<int> accountIds, Spinlock &lock) {

    if (accountIds.size() == 0) {
        return;
    }

    string strIds = "";
    for (int accountId : accountIds) {
        if (strIds.size() > 0) {
            strIds = strIds + ",";
        }
        strIds = strIds + lexical_cast<string>(accountId);
    }

    time_t m_date = get_tmonth(time(nullptr));
    string sPrevMonth = string_date(m_date - 32 * 86400, 6);

    BDbResult res = db->query(
            "   select c.id, COALESCE(a.a_sum,0), c.amount_date " \
            "   from billing.clients c " \
            "   left join " \
            "       (   select account_id, sum(cost) a_sum " \
            "           from calls_raw.calls_raw c " \
            "           left join billing.clients cl on c.account_id=cl.id " \
            "           where " \
            "               c.connect_time >= '" + sPrevMonth + "'::date " \
            "               and c.connect_time >= cl.amount_date " \
            "               and c.account_id in (" + strIds + ") " \
            "           group by account_id " \
            "       ) as a " \
            "   on c.id = a.account_id " \
            "   where c.id in (" + strIds + ") ");


    lock_guard<Spinlock> guard(lock);

    while (res.next()) {
        auto iCc = statsAccount.find(res.get_i(0));
        if (iCc != statsAccount.end() || abs(res.get_d(1)) > 0.00001) {
            StatsAccount &stats = statsAccount[res.get_i(0)];
            stats.account_id = res.get_i(0);
            stats.sum = res.get_d(1);
            stats.amount_date = parseDateTime(res.get(2));
            forSync.insert(stats.account_id);
        }
    }
}

size_t StatsAccountManager::size() {
    return statsAccount.size();
}

StatsAccount * StatsAccountManager::get(int account_id) {
    auto it = statsAccount.find(account_id);
    if (it != statsAccount.end()) {
        return &it->second;
    } else {
        return nullptr;
    }
}

void StatsAccountManager::prepareSaveQuery(stringstream &query) {

    if (realtimeStatsAccountParts[0].size() == 0) {
        return;
    }

    query << "INSERT INTO billing.stats_account(account_id, amount_month, sum_month, amount_day, sum_day, amount_date, sum) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsAccountParts[0]) {
        StatsAccount &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << stats.account_id << "',";
        query << "'" << string_date(stats.amount_month, 1) << "',";
        query << "'" << stats.sum_month << "',";
        query << "'" << string_date(stats.amount_day, 2) << "',";
        query << "'" << stats.sum_day << "',";
        query << "'" << string_time(stats.amount_date, 3) << "',";
        query << "'" << stats.sum << "')";
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

    if (callInfo->account == nullptr || abs(call->cost) < 0.000001) {
        return;
    }


    StatsAccount &stats = statsAccount[call->account_id];

    if (stats.account_id == 0) {
        stats.account_id = call->account_id;
        stats.amount_month = callInfo->dtUtc.month;
        stats.amount_day = callInfo->dtUtc.day;
        stats.amount_date = callInfo->account->amount_date;
    }

    if (abs(callInfo->dtUtc.month - stats.amount_month) < 43200) {
        stats.sum_month += call->cost;
    } else if (callInfo->dtUtc.month > stats.amount_month) {
        stats.amount_month = callInfo->dtUtc.month;
        stats.sum_month = call->cost;
    }

    if (abs(callInfo->dtUtc.day - stats.amount_day) < 43200) {
        stats.sum_day += call->cost;
    } else if (callInfo->dtUtc.day > stats.amount_day) {
        stats.amount_day = callInfo->dtUtc.day;
        stats.sum_day = call->cost;
    }

    if (call->connect_time >= callInfo->account->amount_date) {
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

void StatsAccountManager::getChanges(map<int, StatsAccount> &changes, bool &needClear) {
    needClear = this->needClear;
    if (needClear) {
        for (auto it : statsAccount) {
            changes[it.first] = it.second;
        }
    } else {
        for (int id : forSync) {
            changes[id] = statsAccount[id];
        }
    }
    forSync.clear();
}

void StatsAccountManager::addChanges(map<int, StatsAccount> &changes) {
    for (auto it : changes) {
        forSync.insert(it.first);
    }
}

size_t StatsAccountManager::sync(BDb * db_main, DataBillingContainer * billingData) {

    map<int, StatsAccount> changes;
    bool needClear;
    billingData->statsAccountGetChanges(changes, needClear);

    stringstream query;
    query << "INSERT INTO billing.stats_account(server_id, account_id, amount_month, sum_month, amount_day, sum_day, amount_date, sum) VALUES\n";
    int i = 0;

    for (auto it: changes) {
        StatsAccount &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << app().conf.instance_id << "',";
        query << "'" << stats.account_id << "',";
        query << "'" << string_date(stats.amount_month, 7) << "',";
        query << "'" << stats.sum_month << "',";
        query << "'" << string_date(stats.amount_day, 8) << "',";
        query << "'" << stats.sum_day << "',";
        query << "'" << string_time(stats.amount_date, 4) << "',";
        query << "'" << stats.sum << "')";
        i++;
    }

    if (changes.size() > 0) {
        try {
            if (needClear) {
                BDbTransaction trans(db_main);
                db_main->exec("DELETE FROM billing.stats_account WHERE server_id = " + app().conf.str_instance_id);
                db_main->exec(query.str());
                trans.commit();
            } else {
                db_main->exec(query.str());
            }

        } catch (Exception &e) {
            billingData->statsAccountAddChanges(changes);
            e.addTrace("StatsAccountManager::sync:");
            throw e;
        }
    }

    return changes.size();
}