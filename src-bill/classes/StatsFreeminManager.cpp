#include "StatsFreeminManager.h"
#include "../models/CallInfo.h"

StatsFreeminManager::StatsFreeminManager() {
    realtimeStatsFreeminParts.push_back(map<int, StatsFreemin>());
    realtimeFreeminsByServiceIdParts.push_back(map<int, list<int>>());
}

void StatsFreeminManager::load(BDb * db) {

    BDbResult resId = db->query("   select max(id) from billing.stats_freemin ");
    lastStatsFreeminId = resId.next() ? resId.get_i(0) : 1;

    time_t filterFrom = lastStoredCallTime;
    if (filterFrom > 0) {
        filterFrom = filterFrom - 86400 * 60;
    }

    BDbResult res = db->query(
            "   select id, service_number_id, extract(epoch from month_dt), used_seconds, used_credit, paid_seconds " \
            "   from billing.stats_freemin " \
            "   where month_dt >= '" + string_time(filterFrom) + "'" \
            "   order by month_dt asc"
    );
    while (res.next()) {

        StatsFreemin &stats = storedStatsFreemin[res.get_i(0)];
        stats.id = res.get_i(0);
        stats.service_number_id = res.get_i(1);
        stats.month_dt = res.get_ll(2);
        stats.used_seconds = res.get_i(3);
        stats.used_credit = res.get_d(4);
        stats.paid_seconds = res.get_i(5);

        storedFreeminsByServiceId[stats.service_number_id].push_front(stats.id);
    }

    loaded = true;
}

int StatsFreeminManager::getSeconds(Call * call) {
    lock_guard<Spinlock> guard(lock);

    int seconds = getSeconds(call, storedFreeminsByServiceId, storedStatsFreemin);

    size_t n = 0;
    for (map<int, StatsFreemin> &realtimeStatsFreemin : realtimeStatsFreeminParts) {
        seconds += getSeconds(call, realtimeFreeminsByServiceIdParts[n], realtimeStatsFreeminParts[n]);
        n++;
    }

    return seconds;
}

void StatsFreeminManager::add(CallInfo * callInfo) {

    if (callInfo->call->number_service_id == 0 || callInfo->call->service_package_id != 1 || callInfo->call->package_time == 0) {
        return;
    }

    lock_guard<Spinlock> guard(lock);

    if (!updateStatsFreemin(callInfo->call)) {
        createStatsFreemin(callInfo->call);
    }
}

size_t StatsFreeminManager::size() {
    lock_guard<Spinlock> guard(lock);

    size_t size = storedStatsFreemin.size();

    for (map<int, StatsFreemin> &realtimeStatsFreemin : realtimeStatsFreeminParts) {
        size += realtimeStatsFreemin.size();
    }

    return size;
}

void StatsFreeminManager::save(BDb * dbCalls) {

    size_t parts = realtimeStatsFreeminParts.size();
    if (parts < 2) {
        return;
    }

    stringstream q;
    q << "INSERT INTO billing.stats_freemin(id, month_dt, service_number_id, used_seconds, paid_seconds, used_credit) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsFreeminParts[0]) {
        StatsFreemin &stats = it.second;
        if (i > 0) q << ",\n";
        q << "(";
        q << "'" << stats.id << "',";
        q << "'" << string_time(stats.month_dt) << "',";
        q << "'" << stats.service_number_id << "',";
        q << "'" << stats.used_seconds << "',";
        q << "'" << stats.paid_seconds << "',";
        q << "'" << stats.used_credit << "')";
        i++;
    }

    dbCalls->exec(q.str());
}

void StatsFreeminManager::createNewPartition() {
    lock_guard<Spinlock> guard(lock);

    realtimeStatsFreeminParts.push_back(map<int, StatsFreemin>());
    realtimeFreeminsByServiceIdParts.push_back(map<int, list<int>>());
}

void StatsFreeminManager::afterSave() {
    lock_guard<Spinlock> guard(lock);

    size_t parts = realtimeStatsFreeminParts.size();
    if (parts > 1) {
        if (parts == 2) {
            realtimeStatsFreeminParts.push_back(map<int, StatsFreemin>());
            realtimeFreeminsByServiceIdParts.push_back(map<int, list<int>>());
        }
        move(realtimeStatsFreeminParts[0], realtimeFreeminsByServiceIdParts[0], storedStatsFreemin, storedFreeminsByServiceId);
        realtimeStatsFreeminParts.erase(realtimeStatsFreeminParts.begin());
        realtimeFreeminsByServiceIdParts.erase(realtimeFreeminsByServiceIdParts.begin());
    }
}

int StatsFreeminManager::getSeconds(Call * call, map<int, list<int>> &freeminsByServiceId, map<int, StatsFreemin> &statsFreemin) {
    auto itFreeminsByServiceId = freeminsByServiceId.find(call->number_service_id);
    if (itFreeminsByServiceId == freeminsByServiceId.end()) {
        return 0;
    }

    for (int freeminId : itFreeminsByServiceId->second) {
        auto itStatsFreemin = statsFreemin.find(freeminId);
        if (itStatsFreemin == statsFreemin.end()) continue;

        StatsFreemin &stats = itStatsFreemin->second;

        if (stats.month_dt != call->dt.month) continue;

        return stats.used_seconds;
    }
    return 0;
}

void StatsFreeminManager::createStatsFreemin(Call * call) {

    lastStatsFreeminId += 1;

    size_t parts = realtimeStatsFreeminParts.size();
    map<int, StatsFreemin> &realtimeStatsFreemin = realtimeStatsFreeminParts.at(parts - 1);
    map<int, list<int>> &realtimeFreeminsByServiceId = realtimeFreeminsByServiceIdParts.at(parts - 1);

    StatsFreemin &stats = realtimeStatsFreemin[lastStatsFreeminId];
    stats.id = lastStatsFreeminId;
    stats.service_number_id = call->number_service_id;
    stats.month_dt = call->dt.month;
    stats.used_seconds = call->package_time;
    stats.used_credit = call->package_credit;
    stats.paid_seconds = 123;

    realtimeFreeminsByServiceId[stats.service_number_id].push_front(stats.id);
}

bool StatsFreeminManager::updateStatsFreemin(Call * call) {
    size_t parts = realtimeStatsFreeminParts.size();
    map<int, StatsFreemin> &realtimeStatsFreemin = realtimeStatsFreeminParts.at(parts - 1);
    map<int, list<int>> &realtimeFreeminsByServiceId = realtimeFreeminsByServiceIdParts.at(parts - 1);

    auto itRealtimeFreeminsByServiceId = realtimeFreeminsByServiceId.find(call->number_service_id);
    if (itRealtimeFreeminsByServiceId == realtimeFreeminsByServiceId.end()) {
        return false;
    }

    for (int freeminId : itRealtimeFreeminsByServiceId->second) {
        auto itRealtimeStatsFreemin = realtimeStatsFreemin.find(freeminId);
        if (itRealtimeStatsFreemin == realtimeStatsFreemin.end()) continue;

        StatsFreemin &stats = itRealtimeStatsFreemin->second;

        if (stats.month_dt != call->dt.month) continue;

        stats.used_seconds += call->package_time;
        stats.used_credit += call->package_credit;

        return true;
    }
    return false;
}

void StatsFreeminManager::move(
        map<int, StatsFreemin> &fromStatsFreemin, map<int, list<int>> &fromFreeminsByServiceId,
        map<int, StatsFreemin> &toStatsFreemin, map<int, list<int>> &toFreeminsByServiceId
) {
    for (pair<const int, StatsFreemin> &fromStats : fromStatsFreemin) {
        StatsFreemin &toStats = toStatsFreemin[fromStats.first];
        if (toStats.id == 0) {
            toStats.id = fromStats.second.id;
            toStats.service_number_id = fromStats.second.service_number_id;
            toStats.month_dt = fromStats.second.month_dt;
            toStats.used_seconds = fromStats.second.used_seconds;
            toStats.used_credit = fromStats.second.used_credit;
            toStats.paid_seconds = fromStats.second.paid_seconds;

            toFreeminsByServiceId[toStats.service_number_id].push_front(toStats.id);
        } else {
            toStats.used_seconds += fromStats.second.used_seconds;
            toStats.used_credit += fromStats.second.used_credit;
        }
    }

    fromStatsFreemin.clear();
    fromFreeminsByServiceId.clear();
}
