#include "StatsFreeminManager.h"
#include "Exception.h"
#include "AppBill.h"


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
            "   where month_dt >= '" + string_time(filterFrom, 5) + "'" \
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
}\

void StatsFreeminManager::recalc(BDb * db, long long int storedLastId) {
    string strStoredLastId = lexical_cast<string>(storedLastId);

    db->exec("DELETE FROM billing.stats_freemin WHERE min_call_id > " + strStoredLastId);

    BDbResult res = db->query("select min(min_call_id) from billing.stats_freemin where max_call_id >= " + strStoredLastId);
    int minCallId = res.next() ? res.get_i(0) : 0;

    if (minCallId > 0) {
        string strMinCallId = lexical_cast<string>(minCallId);

        BDbTransaction trans(db);

        db->exec(    "    update billing.stats_freemin set min_call_id = 0 where max_call_id >= " + strStoredLastId);

        db->exec(    "    update billing.stats_freemin p                                    " \
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
                     "        where id >= " + strMinCallId + " and service_package_id = 1   " \
                     "        group by service_package_stats_id                             " \
                     "    ) s                                                               " \
                     "    where p.max_call_id >= " + strStoredLastId + " and p.id = s.id    ");

        db->exec(    "    delete from billing.stats_freemin where min_call_id = 0           ");

        trans.commit();
    }
}

StatsFreemin * StatsFreeminManager::getCurrent(CallInfo * callInfo) {
    auto itFreeminsByServiceId = freeminsByServiceId.find(callInfo->call->number_service_id);
    if (itFreeminsByServiceId != freeminsByServiceId.end()) {
        for (int freeminId : itFreeminsByServiceId->second) {
            auto itStatsFreemin = statsFreemin.find(freeminId);
            if (itStatsFreemin == statsFreemin.end()) continue;

            StatsFreemin &stats = itStatsFreemin->second;

            if (stats.month_dt != callInfo->dt.month) continue;

            return &stats;
        }
    }

    StatsFreemin * stats = createStatsFreemin(callInfo);

    size_t parts = realtimeStatsFreeminParts.size();
    map<int, StatsFreemin> &realtimeStatsFreemin = realtimeStatsFreeminParts.at(parts - 1);
    StatsFreemin &stats2 = realtimeStatsFreemin[stats->id];
    memcpy(&stats2, stats, sizeof(StatsFreemin));
    forSync.insert(stats->id);

    return stats;
}

void StatsFreeminManager::add(CallInfo * callInfo) {

    if (    callInfo->call->number_service_id == 0 ||
            callInfo->call->service_package_id != 1 ||
            callInfo->call->service_package_stats_id == 0
    ) {
        return;
    }

    StatsFreemin * stats = updateStatsFreemin(callInfo, callInfo->call->service_package_stats_id);
    if (stats != nullptr) {
        size_t parts = realtimeStatsFreeminParts.size();
        map<int, StatsFreemin> &realtimeStatsFreemin = realtimeStatsFreeminParts.at(parts - 1);
        StatsFreemin &stats2 = realtimeStatsFreemin[stats->id];
        memcpy(&stats2, stats, sizeof(StatsFreemin));

        forSync.insert(stats->id);
    }
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
        query << "'" << string_time(stats.month_dt, 6) << "',";
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

StatsFreemin * StatsFreeminManager::createStatsFreemin(CallInfo * callInfo) {

    lastStatsFreeminId += 1;

    StatsFreemin &stats = statsFreemin[lastStatsFreeminId];
    stats.id = lastStatsFreeminId;
    stats.service_number_id = callInfo->call->number_service_id;
    stats.month_dt = callInfo->dt.month;
    stats.used_seconds = 0;
    stats.used_credit = 0;
    stats.min_call_id = callInfo->call->id;
    stats.max_call_id = callInfo->call->id;


    freeminsByServiceId[stats.service_number_id].push_front(stats.id);

    return &stats;
}


StatsFreemin * StatsFreeminManager::updateStatsFreemin(CallInfo * callInfo, int statFreeminId) {

    auto itStatsFreemin = statsFreemin.find(statFreeminId);
    if (itStatsFreemin == statsFreemin.end()) {
        return nullptr;
    }

    StatsFreemin &stats = itStatsFreemin->second;
    stats.used_seconds += callInfo->call->package_time;
    stats.used_credit += callInfo->call->package_credit;
    stats.min_call_id = stats.min_call_id == 0 ? callInfo->call->id : stats.min_call_id;
    stats.max_call_id = callInfo->call->id;

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

size_t StatsFreeminManager::sync(BDb * db_main, BDb * db_calls) {

    BDbResult resMax = db_main->query("SELECT max(max_call_id) FROM billing.stats_freemin WHERE server_id='" + app().conf.str_instance_id +"'");
    long long int central_max_call_id = resMax.next() ? resMax.get_ll(0) : 0;

    BDbResult res = db_calls->query(
            " SELECT id, month_dt, service_number_id, used_seconds, used_credit, min_call_id, max_call_id " \
            " from billing.stats_freemin " \
            " where max_call_id > '" + lexical_cast<string>(central_max_call_id) + "'");

    stringstream query;
    query << "INSERT INTO billing.stats_freemin(server_id, id, month_dt, service_number_id, used_seconds, used_credit, min_call_id, max_call_id) VALUES\n";

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
        query << "'" << res.get(6) << "')";
        i++;
    }

    if (res.size() > 0) {
        try {
            db_main->exec(query.str());
        } catch (Exception &e) {
            e.addTrace("StatsFreeminManager::sync:");
            throw e;
        }
    }

    return res.size();
}