#include "StatsPackageManager.h"

StatsPackageManager::StatsPackageManager() {
    realtimeStatsPackageParts.push_back(map<int, StatsPackage>());
    realtimeStatsByPackageIdParts.push_back(map<int, list<int>>());
}

void StatsPackageManager::load(BDb * db) {
    BDbResult resId = db->query("   select max(id) from billing.stats_package p ");
    lastPackageStatId = resId.next() ? resId.get_i(0) : 1;

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
        StatsPackage &stats = storedStatsPackage[res.get_i(0)];
        stats.id = res.get_i(0);
        stats.package_id = res.get_i(1);
        stats.used_seconds = res.get_i(2);
        stats.used_credit = res.get_d(3);
        stats.paid_seconds = res.get_i(4);
        stats.activation_dt = res.get_ll(5);
        stats.expire_dt = res.get_ll(6);
        stats.min_call_id = res.get_ll(7);
        stats.max_call_id = res.get_ll(8);

        storedStatsByPackageId[stats.package_id].push_front(stats.id);
    }

    loaded = true;
}

int StatsPackageManager::getSeconds(Call * call) {
    lock_guard<Spinlock> guard(lock);

    int seconds = getSeconds(call->service_package_id, call->connect_time, storedStatsByPackageId, storedStatsPackage);

    size_t n = 0;
    for (map<int, StatsPackage> &realtimeStatsPackage : realtimeStatsPackageParts) {
        seconds += getSeconds(call->service_package_id, call->connect_time, realtimeStatsByPackageIdParts[n], realtimeStatsPackageParts[n]);
        n++;
    }

    return seconds;
}

int StatsPackageManager::getSeconds(int service_package_id, time_t connect_time) {
    lock_guard<Spinlock> guard(lock);

    int seconds = getSeconds(service_package_id, connect_time, storedStatsByPackageId, storedStatsPackage);

    size_t n = 0;
    for (map<int, StatsPackage> &realtimeStatsPackage : realtimeStatsPackageParts) {
        seconds += getSeconds(service_package_id, connect_time, realtimeStatsByPackageIdParts[n], realtimeStatsPackageParts[n]);
        n++;
    }

    return seconds;
}

void StatsPackageManager::add(CallInfo * callInfo) {

    if (callInfo->call->number_service_id == 0 || callInfo->call->service_package_id <= 1 || callInfo->call->package_time == 0) {
        return;
    }

    lock_guard<Spinlock> guard(lock);

    if (!updateStatsPackage(callInfo)) {
        createStatsPackage(callInfo);
    }
}

size_t StatsPackageManager::size() {
    lock_guard<Spinlock> guard(lock);


    size_t size = storedStatsPackage.size();

    for (map<int, StatsPackage> &realtimeStatsPackage : realtimeStatsPackageParts) {
        size += realtimeStatsPackage.size();
    }

    return size;
}

void StatsPackageManager::save(BDb * dbCalls) {

    size_t parts = realtimeStatsPackageParts.size();
    if (parts < 2) {
        return;
    }

    stringstream q;
    q << "INSERT INTO billing.stats_package(id, package_id, used_seconds, used_credit, paid_seconds, activation_dt, expire_dt, min_call_id, max_call_id) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsPackageParts[0]) {
        StatsPackage &stats = it.second;
        if (i > 0) q << ",\n";
        q << "(";
        q << "'" << stats.id << "',";
        q << "'" << stats.package_id << "',";
        q << "'" << stats.used_seconds << "',";
        q << "'" << stats.used_credit << "',";
        q << "'" << stats.paid_seconds << "',";
        q << "'" << string_time(stats.activation_dt) << "',";
        q << "'" << string_time(stats.expire_dt) << "',";
        q << "'" << stats.min_call_id << "',";
        q << "'" << stats.max_call_id << "')";
        i++;
    }

    dbCalls->exec(q.str());
}

void StatsPackageManager::createNewPartition() {
    lock_guard<Spinlock> guard(lock);

    realtimeStatsPackageParts.push_back(map<int, StatsPackage>());
    realtimeStatsByPackageIdParts.push_back(map<int, list<int>>());
}

void StatsPackageManager::afterSave() {
    lock_guard<Spinlock> guard(lock);

    size_t parts = realtimeStatsPackageParts.size();
    if (parts > 1) {
        if (parts == 2) {
            realtimeStatsPackageParts.push_back(map<int, StatsPackage>());
            realtimeStatsByPackageIdParts.push_back(map<int, list<int>>());
        }
        move(realtimeStatsPackageParts[0], realtimeStatsByPackageIdParts[0], storedStatsPackage, storedStatsByPackageId);
        realtimeStatsPackageParts.erase(realtimeStatsPackageParts.begin());
        realtimeStatsByPackageIdParts.erase(realtimeStatsByPackageIdParts.begin());
    }
}

int StatsPackageManager::getSeconds(int service_package_id, time_t connect_time, map<int, list<int>> &statsByPackageId, map<int, StatsPackage> &statsPackage) {
    auto itStatsByPackageId= statsByPackageId.find(service_package_id);
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

void StatsPackageManager::createStatsPackage(CallInfo *callInfo) {
    short timezone_offset = 0;


    size_t parts = realtimeStatsPackageParts.size();
    map<int, StatsPackage> &realtimeStatsPackage = realtimeStatsPackageParts.at(parts - 1);
    map<int, list<int>> &realtimeStatsByPackageId = realtimeStatsByPackageIdParts.at(parts - 1);

    time_t activation_dt = get_tmonth(callInfo->call->connect_time, timezone_offset);
    time_t expire_dt = get_tmonth_end(callInfo->call->connect_time, timezone_offset);

    if (activation_dt < callInfo->servicePackagePrepaid->activation_dt) {
        activation_dt = callInfo->servicePackagePrepaid->activation_dt;
    }

    if (expire_dt > callInfo->servicePackagePrepaid->expire_dt) {
        expire_dt = callInfo->servicePackagePrepaid->expire_dt;
    }

    lastPackageStatId += 1;

    StatsPackage &stats = realtimeStatsPackage[lastPackageStatId];
    stats.id = lastPackageStatId;
    stats.package_id = callInfo->call->service_package_id;
    stats.activation_dt = activation_dt;
    stats.expire_dt = expire_dt;
    stats.used_seconds = callInfo->call->package_time;
    stats.used_credit = callInfo->call->package_credit;
    stats.paid_seconds = callInfo->tariffPackagePrepaid->getPrepaidSeconds();
    stats.min_call_id = callInfo->call->id;
    stats.max_call_id = callInfo->call->id;

    realtimeStatsByPackageId[stats.package_id].push_front(stats.id);
}

bool StatsPackageManager::updateStatsPackage(CallInfo *callInfo) {
    size_t parts = realtimeStatsPackageParts.size();
    map<int, StatsPackage> &realtimeStatsPackage = realtimeStatsPackageParts.at(parts - 1);
    map<int, list<int>> &realtimeStatsByPackageId = realtimeStatsByPackageIdParts.at(parts - 1);

    auto itRealtimeStatsByPackageId = realtimeStatsByPackageId.find(callInfo->call->service_package_id);
    if (itRealtimeStatsByPackageId == realtimeStatsByPackageId.end()) {
        return false;
    }

    for (int packageId : itRealtimeStatsByPackageId->second) {
        auto itRealtimeStatsPackage = realtimeStatsPackage.find(packageId);
        if (itRealtimeStatsPackage == realtimeStatsPackage.end()) continue;

        StatsPackage &stats = itRealtimeStatsPackage->second;

        if (stats.activation_dt > callInfo->call->connect_time) continue;
        if (stats.expire_dt < callInfo->call->connect_time) continue;

        stats.used_seconds += callInfo->call->package_time;
        stats.used_credit += callInfo->call->package_credit;
        stats.max_call_id = callInfo->call->id;

        return true;
    }

    return false;
}

void StatsPackageManager::move(
        map<int, StatsPackage> &fromStatsPackage, map<int, list<int>> &fromStatsByPackageId,
        map<int, StatsPackage> &toStatsPackage, map<int, list<int>> &toStatsByPackageId
) {
    for (pair<const int, StatsPackage> &fromStats : fromStatsPackage) {
        StatsPackage &toStats = toStatsPackage[fromStats.first];
        if (toStats.id == 0) {
            toStats.id = fromStats.second.id;
            toStats.package_id = fromStats.second.package_id;
            toStats.activation_dt = fromStats.second.activation_dt;
            toStats.expire_dt = fromStats.second.expire_dt;
            toStats.used_seconds = fromStats.second.used_seconds;
            toStats.used_credit = fromStats.second.used_credit;
            toStats.paid_seconds = fromStats.second.paid_seconds;
            toStats.min_call_id = fromStats.second.min_call_id;
            toStats.max_call_id = fromStats.second.max_call_id;

            toStatsByPackageId[toStats.package_id].push_front(toStats.id);
        } else {
            toStats.used_seconds += fromStats.second.used_seconds;
            toStats.used_credit += fromStats.second.used_credit;
            toStats.max_call_id = fromStats.second.max_call_id;
        }
    }

    fromStatsPackage.clear();
    fromStatsByPackageId.clear();
}
