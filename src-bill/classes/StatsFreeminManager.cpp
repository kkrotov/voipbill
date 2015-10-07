#include "StatsFreeminManager.h"
#include "../models/CallInfo.h"

StatsFreeminManager::StatsFreeminManager() {
    realtimeStatsFreeminParts.push_back(map<int, StatsFreemin>());
}

void StatsFreeminManager::load(BDb * db, time_t lastStoredCallTime) {
    forSync.clear();
    statsFreemin.clear();
    freeminsByServiceId.clear();
    realtimeStatsFreeminParts.clear();
    realtimeStatsFreeminParts.push_back(map<int, StatsFreemin>());

    BDbResult resId = db->query("   select max(id) from billing.stats_freemin ");
    lastStatsFreeminId = resId.next() ? resId.get_i(0) : 0;

    time_t filterFrom = lastStoredCallTime;
    if (filterFrom > 0) {
        filterFrom = filterFrom - 86400 * 60;
    }

    BDbResult res = db->query(
            "   select id, service_number_id, extract(epoch from month_dt), used_seconds, used_credit, min_call_id, max_call_id " \
            "   from billing.stats_freemin " \
            "   where month_dt >= '" + string_time(filterFrom) + "'" \
            "   order by month_dt asc"
    );
    while (res.next()) {
        StatsFreemin &stats = statsFreemin[res.get_i(0)];
        stats.id = res.get_i(0);
        stats.service_number_id = res.get_i(1);
        stats.month_dt = res.get_ll(2);
        stats.used_seconds = res.get_i(3);
        stats.used_credit = res.get_d(4);
        stats.min_call_id = res.get_ll(5);
        stats.max_call_id = res.get_ll(6);

        freeminsByServiceId[stats.service_number_id].push_front(stats.id);
    }

    loaded = true;
}

void StatsFreeminManager::recalc(BDb * db, long long int storedLastId) {
    db->exec("DELETE FROM billing.stats_freemin WHERE max_call_id > " + lexical_cast<string>(storedLastId));
}

int StatsFreeminManager::getSeconds(Call * call) {
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

void StatsFreeminManager::add(CallInfo * callInfo) {

    if (callInfo->call->number_service_id == 0 || callInfo->call->service_package_id != 1) {
        return;
    }

    int statFreeminId = getStatsFreeminId(callInfo->call);
    StatsFreemin * stats;

    if (statFreeminId > 0) {
        stats = updateStatsFreemin(callInfo->call, statFreeminId);
    } else {
        stats = createStatsFreemin(callInfo->call);
    }

    size_t parts = realtimeStatsFreeminParts.size();
    map<int, StatsFreemin> &realtimeStatsFreemin = realtimeStatsFreeminParts.at(parts - 1);
    StatsFreemin &stats2 = realtimeStatsFreemin[stats->id];
    memcpy(&stats2, stats, sizeof(StatsFreemin));

    forSync.insert(stats->id);
}

size_t StatsFreeminManager::size() {
    return statsFreemin.size();
}

void StatsFreeminManager::prepareSaveQuery(stringstream &query) {

    if (realtimeStatsFreeminParts[0].size() == 0) {
        return;
    }

    query << "INSERT INTO billing.stats_freemin(id, month_dt, service_number_id, used_seconds, used_credit, min_call_id, max_call_id) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsFreeminParts[0]) {
        StatsFreemin &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << stats.id << "',";
        query << "'" << string_time(stats.month_dt) << "',";
        query << "'" << stats.service_number_id << "',";
        query << "'" << stats.used_seconds << "',";
        query << "'" << stats.used_credit << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }
}

void StatsFreeminManager::executeSaveQuery(BDb * dbCalls, stringstream &query) {
    string q = query.str();
    if (q.size() > 0) {

        dbCalls->exec(q);

    }
}

void StatsFreeminManager::createNewPartition() {
    realtimeStatsFreeminParts.push_back(map<int, StatsFreemin>());
}

void StatsFreeminManager::removePartitionAfterSave() {
    realtimeStatsFreeminParts.erase(realtimeStatsFreeminParts.begin());
}

int StatsFreeminManager::getStatsFreeminId(Call * call) {
    auto itFreeminsByServiceId = freeminsByServiceId.find(call->number_service_id);
    if (itFreeminsByServiceId == freeminsByServiceId.end()) {
        return 0;
    }

    for (int freeminId : itFreeminsByServiceId->second) {
        auto itStatsFreemin = statsFreemin.find(freeminId);
        if (itStatsFreemin == statsFreemin.end()) continue;

        StatsFreemin &stats = itStatsFreemin->second;

        if (stats.month_dt != call->dt.month) continue;

        return stats.id;
    }

    return 0;
}

StatsFreemin * StatsFreeminManager::createStatsFreemin(Call * call) {

    lastStatsFreeminId += 1;

    StatsFreemin &stats = statsFreemin[lastStatsFreeminId];
    stats.id = lastStatsFreeminId;
    stats.service_number_id = call->number_service_id;
    stats.month_dt = call->dt.month;
    stats.used_seconds = call->package_time;
    stats.used_credit = call->package_credit;
    stats.min_call_id = call->id;
    stats.max_call_id = call->id;


    freeminsByServiceId[stats.service_number_id].push_front(stats.id);

    return &stats;
}


StatsFreemin * StatsFreeminManager::updateStatsFreemin(Call * call, int statFreeminId) {

    StatsFreemin &stats = statsFreemin[statFreeminId];
    stats.used_seconds += call->package_time;
    stats.used_credit += call->package_credit;
    stats.max_call_id = call->id;

    return &stats;
}

void StatsFreeminManager::getChanges(map<int, StatsFreemin> &changes) {
    for (int id : forSync) {
        changes[id] = statsFreemin[id];
    }
    forSync.clear();
}

void StatsFreeminManager::addChanges(map<int, StatsFreemin> &changes) {
    for (auto it : changes) {
        forSync.insert(it.first);
    }
}