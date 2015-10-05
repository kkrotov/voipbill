#include "StatsPackageManager.h"

StatsPackageManager::StatsPackageManager() {
    realtimeStatsPackageParts.push_back(map<int, StatsPackage>());
}

void StatsPackageManager::load(BDb * db) {
    BDbResult resId = db->query("   select max(id) from billing.stats_package p ");
    lastPackageStatId = resId.next() ? resId.get_i(0) : 0;

    time_t filterFrom = lastSaveCallTime;
    if (filterFrom > 0) {
        filterFrom = filterFrom - 86400 * 60;
    }

    BDbResult res = db->query(
            "   select id, package_id, used_seconds, used_credit, paid_seconds, extract(epoch from activation_dt), extract(epoch from expire_dt), min_call_id, max_call_id " \
            "   from billing.stats_package " \
            "   where activation_dt >= '" + string_time(filterFrom) + "'" \
            "   order by package_id asc, activation_dt asc "
    );
    while (res.next()) {
        StatsPackage &stats = statsPackage[res.get_i(0)];
        stats.id = res.get_i(0);
        stats.package_id = res.get_i(1);
        stats.used_seconds = res.get_i(2);
        stats.used_credit = res.get_d(3);
        stats.paid_seconds = res.get_i(4);
        stats.activation_dt = res.get_ll(5);
        stats.expire_dt = res.get_ll(6);
        stats.min_call_id = res.get_ll(7);
        stats.max_call_id = res.get_ll(8);

        statsByPackageId[stats.package_id].push_front(stats.id);
    }

    loaded = true;
}

void StatsPackageManager::recalc(BDb * db, long long int storedLastId) {
    db->exec("DELETE FROM billing.stats_package WHERE max_call_id > " + lexical_cast<string>(storedLastId));
}

int StatsPackageManager::getSeconds(int service_package_id, time_t connect_time) {
    auto itStatsByPackageId = statsByPackageId.find(service_package_id);
    if (itStatsByPackageId == statsByPackageId.end()) {
        return 0;
    }

    for (int packageId : itStatsByPackageId->second) {
        auto itStatsPackage = statsPackage.find(packageId);
        if (itStatsPackage == statsPackage.end()) continue;

        StatsPackage &stats = itStatsPackage->second;

        if (stats.activation_dt > connect_time) continue;
        if (stats.expire_dt < connect_time) continue;

        return stats.used_seconds;
    }
    return 0;
}

void StatsPackageManager::add(CallInfo * callInfo) {

    if (
            callInfo->call->number_service_id == 0 ||
            callInfo->call->service_package_id <= 1 ||
            callInfo->servicePackagePrepaid == nullptr
    ) {
        return;
    }

    int statPackageId = getStatsPackageId(callInfo->call);
    StatsPackage * stats;
    if (statPackageId > 0) {
        stats = updateStatsPackage(callInfo->call, statPackageId);
    } else {
        stats = createStatsPackage(callInfo);
    }

    size_t parts = realtimeStatsPackageParts.size();
    map<int, StatsPackage> &realtimeStatsPackage = realtimeStatsPackageParts.at(parts - 1);
    StatsPackage &stats2 = realtimeStatsPackage[stats->id];
    memcpy(&stats2, stats, sizeof(StatsPackage));

    forSync.insert(stats->id);
}

size_t StatsPackageManager::size() {
    return statsPackage.size();
}

void StatsPackageManager::prepareSaveQuery(stringstream &query) {

    if (realtimeStatsPackageParts[0].size() == 0) {
        return;
    }

    query << "INSERT INTO billing.stats_package(id, package_id, used_seconds, used_credit, paid_seconds, activation_dt, expire_dt, min_call_id, max_call_id) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsPackageParts[0]) {
        StatsPackage &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << stats.id << "',";
        query << "'" << stats.package_id << "',";
        query << "'" << stats.used_seconds << "',";
        query << "'" << stats.used_credit << "',";
        query << "'" << stats.paid_seconds << "',";
        query << "'" << string_time(stats.activation_dt) << "',";
        query << "'" << string_time(stats.expire_dt) << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }
}

void StatsPackageManager::executeSaveQuery(BDb * dbCalls, stringstream &query) {
    string q = query.str();
    if (q.size() > 0) {

        dbCalls->exec(q);

    }
}

void StatsPackageManager::createNewPartition() {
    realtimeStatsPackageParts.push_back(map<int, StatsPackage>());
}

void StatsPackageManager::removePartitionAfterSave() {
    realtimeStatsPackageParts.erase(realtimeStatsPackageParts.begin());
}

int StatsPackageManager::getStatsPackageId(Call * call) {
    auto itStatsByPackageId = statsByPackageId.find(call->service_package_id);
    if (itStatsByPackageId == statsByPackageId.end()) {
        return 0;
    }

    for (int freeminId : itStatsByPackageId->second) {
        auto itStatsFreemin = statsPackage.find(freeminId);
        if (itStatsFreemin == statsPackage.end()) continue;

        StatsPackage &stats = itStatsFreemin->second;

        if (stats.activation_dt > call->connect_time) continue;
        if (stats.expire_dt < call->connect_time) continue;

        return stats.id;
    }

    return 0;
}

StatsPackage * StatsPackageManager::createStatsPackage(CallInfo *callInfo) {
    short timezone_offset = 0;

    time_t activation_dt = get_tmonth(callInfo->call->connect_time, timezone_offset);
    time_t expire_dt = get_tmonth_end(callInfo->call->connect_time, timezone_offset);

    if (activation_dt < callInfo->servicePackagePrepaid->activation_dt) {
        activation_dt = callInfo->servicePackagePrepaid->activation_dt;
    }

    if (expire_dt > callInfo->servicePackagePrepaid->expire_dt) {
        expire_dt = callInfo->servicePackagePrepaid->expire_dt;
    }

    lastPackageStatId += 1;

    StatsPackage &stats = statsPackage[lastPackageStatId];
    stats.id = lastPackageStatId;
    stats.package_id = callInfo->call->service_package_id;
    stats.activation_dt = activation_dt;
    stats.expire_dt = expire_dt;
    stats.used_seconds = callInfo->call->package_time;
    stats.used_credit = callInfo->call->package_credit;
    stats.paid_seconds = callInfo->tariffPackagePrepaid->getPrepaidSeconds();
    stats.min_call_id = callInfo->call->id;
    stats.max_call_id = callInfo->call->id;

    statsByPackageId[stats.package_id].push_front(stats.id);

    return &stats;
}

StatsPackage * StatsPackageManager::updateStatsPackage(Call *call, int statPackageId) {

    StatsPackage &stats = statsPackage[statPackageId];

    stats.used_seconds += call->package_time;
    stats.used_credit += call->package_credit;
    stats.max_call_id = call->id;

    return &stats;
}

void StatsPackageManager::getChanges(map<int, StatsPackage> &changes) {
    for (int id : forSync) {
        changes[id] = statsPackage[id];
    }
    forSync.clear();
}

void StatsPackageManager::addChanges(map<int, StatsPackage> &changes) {
    for (auto it : changes) {
        forSync.insert(it.first);
    }
}