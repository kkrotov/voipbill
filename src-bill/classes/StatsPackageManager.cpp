#include "StatsPackageManager.h"
#include "Exception.h"
#include "AppBill.h"

StatsPackageManager::StatsPackageManager() {
    realtimeStatsPackageParts.push_back(map<int, StatsPackage>());
}

void StatsPackageManager::load(BDb * db, time_t lastSaveCallTime) {
    forSync.clear();
    statsPackage.clear();
    statsByPackageId.clear();
    realtimeStatsPackageParts.clear();
    realtimeStatsPackageParts.push_back(map<int, StatsPackage>());

    BDbResult resId = db->query("   select max(id) from billing.stats_package p ");
    lastPackageStatId = resId.next() ? resId.get_i(0) : 0;

    time_t filterFrom = lastSaveCallTime;
    if (filterFrom > 0) {
        filterFrom = filterFrom - 86400 * 60;
    }

    BDbResult res = db->query(
            "   select id, package_id, used_seconds, used_credit, paid_seconds, extract(epoch from activation_dt), extract(epoch from expire_dt), min_call_id, max_call_id " \
            "   from billing.stats_package " \
            "   where activation_dt >= '" + string_time(filterFrom, 7) + "'" \
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
    string strStoredLastId = lexical_cast<string>(storedLastId);

    db->exec("DELETE FROM billing.stats_package WHERE min_call_id > " + strStoredLastId);

    BDbResult res = db->query("select min(min_call_id) from billing.stats_package where max_call_id >= " + strStoredLastId);
    int minCallId = res.next() ? res.get_i(0) : 0;

    if (minCallId > 0) {
        string strMinCallId = lexical_cast<string>(minCallId);

        BDbTransaction trans(db);

        db->exec(    "    update billing.stats_package set min_call_id = 0 where max_call_id >= " + strStoredLastId);

        db->exec(    "    update billing.stats_package p                                    " \
                     "    set                                                               " \
                     "        min_call_id = s.min_call_id,                                  " \
                     "        max_call_id = s.max_call_id,                                  " \
                     "        used_seconds = coalesce(s.used_seconds, 0),                   " \
                     "        used_credit = coalesce(s.used_credit, 0)                      " \
                     "    from (                                                            " \
                     "        select  service_package_stats_id as id,                       " \
                     "                sum(package_time) as used_seconds,                    " \
                     "                sum(package_credit) as used_credit,                   " \
                     "                max(id) as min_call_id, max(id) as max_call_id        " \
                     "        from calls_raw.calls_raw                                      " \
                     "        where id >= " + strMinCallId + " and service_package_id > 1   " \
                     "        group by service_package_stats_id                             " \
                     "    ) s                                                               " \
                     "    where p.max_call_id >= " + strStoredLastId + " and p.id = s.id    ");

        db->exec(    "    delete from billing.stats_package where min_call_id = 0           ");

        trans.commit();
    }
}


StatsPackage * StatsPackageManager::getCurrent(CallInfo * callInfo, ServicePackage * servicePackage, TariffPackage * tariffPackage) {
    auto itStatsByPackageId = statsByPackageId.find(servicePackage->id);
    if (itStatsByPackageId != statsByPackageId.end()) {
        for (int packageId : itStatsByPackageId->second) {
            auto itStatsPackage = statsPackage.find(packageId);
            if (itStatsPackage == statsPackage.end()) continue;

            StatsPackage &stats = itStatsPackage->second;

            if (stats.activation_dt > callInfo->call->connect_time) continue;
            if (stats.expire_dt < callInfo->call->connect_time) continue;

            return &stats;
        }
    }

    StatsPackage * stats = createStatsPackage(callInfo, servicePackage, tariffPackage);

    size_t parts = realtimeStatsPackageParts.size();
    map<int, StatsPackage> &realtimeStatsPackage = realtimeStatsPackageParts.at(parts - 1);
    StatsPackage &stats2 = realtimeStatsPackage[stats->id];
    memcpy(&stats2, stats, sizeof(StatsPackage));
    forSync.insert(stats->id);

    return stats;
}

void StatsPackageManager::add(CallInfo * callInfo) {

    if (
            callInfo->call->number_service_id == 0 ||
            callInfo->call->service_package_id <= 1 ||
            callInfo->call->service_package_stats_id == 0 ||
            callInfo->servicePackagePrepaid == nullptr
    ) {
        return;
    }

    StatsPackage * stats = updateStatsPackage(callInfo, callInfo->call->service_package_stats_id);
    if (stats != nullptr) {
        size_t parts = realtimeStatsPackageParts.size();
        map<int, StatsPackage> &realtimeStatsPackage = realtimeStatsPackageParts.at(parts - 1);
        StatsPackage &stats2 = realtimeStatsPackage[stats->id];
        memcpy(&stats2, stats, sizeof(StatsPackage));

        forSync.insert(stats->id);
    }
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

        if (stats.min_call_id == 0 || stats.max_call_id == 0) {
            continue;
        }

        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << stats.id << "',";
        query << "'" << stats.package_id << "',";
        query << "'" << stats.used_seconds << "',";
        query << "'" << stats.used_credit << "',";
        query << "'" << stats.paid_seconds << "',";
        query << "'" << string_time(stats.activation_dt, 8) << "',";
        query << "'" << string_time(stats.expire_dt, 9) << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }

    if (i == 0) {
        query.clear();
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

StatsPackage * StatsPackageManager::createStatsPackage(CallInfo * callInfo, ServicePackage * servicePackage, TariffPackage * tariffPackage) {

    time_t activation_dt;
    time_t expire_dt;
    int paid_seconds;

    if (servicePackage->periodical) {
        activation_dt = get_tmonth(callInfo->call->connect_time, callInfo->account->timezone_offset);
        expire_dt = get_tmonth_end(callInfo->call->connect_time, callInfo->account->timezone_offset);
        double month_seconds = expire_dt + 1 - activation_dt;

        if (activation_dt < servicePackage->activation_dt) {
            activation_dt = servicePackage->activation_dt;
        }

        if (expire_dt > servicePackage->expire_dt) {
            expire_dt = servicePackage->expire_dt;
        }

        double package_seconds = expire_dt + 1 - activation_dt;

        paid_seconds = (int)round(package_seconds * tariffPackage->getPrepaidSeconds() / month_seconds);

    } else {
        activation_dt = servicePackage->activation_dt;
        expire_dt = servicePackage->expire_dt;
        paid_seconds = tariffPackage->getPrepaidSeconds();
    }

    lastPackageStatId += 1;

    StatsPackage &stats = statsPackage[lastPackageStatId];
    stats.id = lastPackageStatId;
    stats.package_id = servicePackage->id;
    stats.activation_dt = activation_dt;
    stats.expire_dt = expire_dt;
    stats.used_seconds = 0;
    stats.used_credit = 0;
    stats.paid_seconds = paid_seconds;
    stats.min_call_id = 0;
    stats.max_call_id = 0;

    statsByPackageId[stats.package_id].push_front(stats.id);

    return &stats;
}

StatsPackage * StatsPackageManager::updateStatsPackage(CallInfo *callInfo, int statPackageId) {

    auto itStatsPackage = statsPackage.find(statPackageId);
    if (itStatsPackage == statsPackage.end()) {
        return nullptr;
    }

    StatsPackage &stats = itStatsPackage->second;

    stats.used_seconds += callInfo->call->package_time;
    stats.used_credit += callInfo->call->package_credit;
    stats.min_call_id = stats.min_call_id == 0 ? callInfo->call->id : stats.min_call_id;
    stats.max_call_id = callInfo->call->id;

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

size_t StatsPackageManager::sync(BDb * db_main, BDb * db_calls) {

    BDbResult resMax = db_main->query("SELECT max(max_call_id) FROM billing.stats_package WHERE server_id='" + app().conf.str_instance_id +"'");
    long long int central_max_call_id = resMax.next() ? resMax.get_ll(0) : 0;

    BDbResult res = db_calls->query(
            " SELECT id, package_id, used_seconds, used_credit, paid_seconds, activation_dt, expire_dt, min_call_id, max_call_id " \
        " from billing.stats_package " \
        " where max_call_id > '" + lexical_cast<string>(central_max_call_id) + "'");

    stringstream query;
    query << "INSERT INTO billing.stats_package(server_id, id, package_id, used_seconds, used_credit, paid_seconds, activation_dt, expire_dt, min_call_id, max_call_id) VALUES\n";

    int i = 0;
    while (res.next()) {
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << app().conf.instance_id << "',";
        query << "'" << res.get(0) << "',";
        query << "'" << res.get(1) << "',";
        query << "'" << res.get(2) << "',";
        query << "'" << res.get(3) << "',";
        query << "'" << res.get(4) << "',";
        query << "'" << res.get(5) << "',";
        query << "'" << res.get(6) << "',";
        query << "'" << res.get(7) << "',";
        query << "'" << res.get(8) << "')";
        i++;
    }

    if (res.size() > 0) {
        try {
            db_main->exec(query.str());
        } catch (Exception &e) {
            e.addTrace("StatsPackageManager::sync:");
            throw e;
        }
    }

    return res.size();
}