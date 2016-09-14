#include "StatsNNPPackageMinuteManager.h"
#include "AppBill.h"


StatsNNPPackageMinuteManager::StatsNNPPackageMinuteManager() {
    realtimeStatsNNPPackageMinuteParts.push_back(map<int, StatsNNPPackageMinute>());
}

void StatsNNPPackageMinuteManager::load(BDb *db, time_t lastSaveCallTime) {
    forSync.clear();
    statsNNPPackageMinute.clear();
    statsByNNPPackageMinuteId.clear();
    realtimeStatsNNPPackageMinuteParts.clear();
    realtimeStatsNNPPackageMinuteParts.push_back(map<int, StatsNNPPackageMinute>());

    BDbResult resId = db->query("   select max(id) from billing.stats_nnp_package_minute p ");
    lastTrunkNNPPackageMinuteId = resId.next() ? resId.get_i(0) : 0;

    time_t filterFrom = lastSaveCallTime;
    if (filterFrom > 0) {
        filterFrom = filterFrom - 86400 * 60;
    }

    BDbResult res = db->query(
            "   select id,nnp_account_tariff_light_id, nnp_package_minute_id, used_seconds, paid_seconds, used_credit, extract(epoch from activate_from), " \
            "   extract(epoch from deactivate_from), min_call_id, max_call_id " \
            "   from billing.stats_nnp_package_minute " \
            "   where activate_from >= '" + string_time(filterFrom, 7) + "'" \
            "   order by id asc, activate_from asc "
    );
    while (res.next()) {
        StatsNNPPackageMinute &stats = statsNNPPackageMinute[res.get_i(0)];
        stats.id = res.get_i(0);
        stats.nnp_account_tariff_light_id = res.get_i(1);
        stats.nnp_package_minute_id = res.get_i(2);
        stats.used_seconds = res.get_i(3);
        stats.paid_seconds = res.get_i(4);
        stats.used_credit = res.get_d(5);
        stats.activate_from = res.get_ll(6);
        stats.deactivate_from = res.get_ll(7);
        stats.min_call_id = res.get_ll(8);
        stats.max_call_id = res.get_ll(9);

        statsByNNPPackageMinuteId[stats.id].push_front(stats.id);
    }

    loaded = true;
}

void StatsNNPPackageMinuteManager::recalc(BDb *db, long long int storedLastId) {
    string strStoredLastId = lexical_cast<string>(storedLastId);

    db->exec("DELETE FROM billing.stats_nnp_package_minute WHERE min_call_id > " + strStoredLastId);

    BDbResult res = db->query(
            "select min(min_call_id) from billing.stats_nnp_package_minute where max_call_id >= " + strStoredLastId);
    int minCallId = res.next() ? res.get_i(0) : 0;

    if (minCallId > 0) {
        string strMinCallId = lexical_cast<string>(minCallId);

        BDbTransaction trans(db);

        db->exec("    update billing.stats_nnp_package_minute set min_call_id = 0 where max_call_id >= " +
                 strStoredLastId);

        db->exec("    update billing.stats_nnp_package_minute p                             " \
                     "    set                                                               " \
                     "        min_call_id = s.min_call_id,                                  " \
                     "        max_call_id = s.max_call_id,                                  " \
                     "        used_seconds = coalesce(s.used_seconds, 0),                   " \
                     "        used_credit = coalesce(s.used_credit, 0)                      " \
                     "    from (                                                            " \
                     "        select  stats_nnp_package_minute_orig_id as id,               " \
                     "                sum(billed_time) as used_seconds,                     " \
                     "                sum(cost) as used_credit,                             " \
                     "                max(id) as min_call_id, max(id) as max_call_id        " \
                     "        from calls_raw.calls_raw                                      " \
                     "        where id >= " + strMinCallId + "                              " \
                     "        group by stats_nnp_package_minute_orig_id                     " \
                     "    ) s                                                               " \
                     "    where p.max_call_id >= " + strStoredLastId + " and p.id = s.id    ");

        db->exec("    delete from billing.stats_trunk_settings where min_call_id = 0    ");

        trans.commit();
    }
}

StatsNNPPackageMinute *StatsNNPPackageMinuteManager::getCurrent(time_t connect_time, Client *account,
                                                                StatsNNPPackageMinute *nnpPackageMinute) {
    auto itByNNPPackageMinuteId = statsByNNPPackageMinuteId.find(nnpPackageMinute->id);
    if (itByNNPPackageMinuteId != statsByNNPPackageMinuteId.end()) {
        for (int nnpPackageMinuteId : itByNNPPackageMinuteId->second) {
            auto itNNPPackageMinute = statsNNPPackageMinute.find(nnpPackageMinuteId);
            if (itNNPPackageMinute == statsNNPPackageMinute.end()) continue;

            StatsNNPPackageMinute &stats = itNNPPackageMinute->second;

            if (stats.activate_from > connect_time) continue;
            if (stats.deactivate_from < connect_time) continue;

            return &stats;
        }
    }

    StatsNNPPackageMinute *stats = createStatsNNPPackageMinute(connect_time, account, nnpPackageMinute);

    size_t parts = realtimeStatsNNPPackageMinuteParts.size();
    map<int, StatsNNPPackageMinute> &realtimeStatsNNPPackageMinute = realtimeStatsNNPPackageMinuteParts.at(parts - 1);
    StatsNNPPackageMinute &stats2 = realtimeStatsNNPPackageMinute[stats->id];
    memcpy(&stats2, stats, sizeof(StatsNNPPackageMinute));
    forSync.insert(stats->id);

    return stats;
}

void StatsNNPPackageMinuteManager::add(CallInfo *callInfo) {

    if (callInfo->call->stats_nnp_package_minute_id == 0) {
        return;
    }

    StatsNNPPackageMinute *stats = updateStatsNNPPackageMinute(callInfo, callInfo->call->stats_nnp_package_minute_id);
    if (stats != nullptr) {
        size_t parts = realtimeStatsNNPPackageMinuteParts.size();
        map<int, StatsNNPPackageMinute> &realtimeStatsNNPPackageMinute = realtimeStatsNNPPackageMinuteParts.at(
                parts - 1);
        StatsNNPPackageMinute &stats2 = realtimeStatsNNPPackageMinute[stats->id];
        memcpy(&stats2, stats, sizeof(StatsNNPPackageMinute));

        forSync.insert(stats->id);
    }
}

size_t StatsNNPPackageMinuteManager::size() {
    return statsNNPPackageMinute.size();
}

void StatsNNPPackageMinuteManager::prepareSaveQuery(stringstream &query) {

    if (realtimeStatsNNPPackageMinuteParts[0].size() == 0) {
        return;
    }

    query <<

    "INSERT INTO billing.stats_nnp_package_minute(id,nnp_account_tariff_light_id, nnp_package_minute_id, used_seconds, " \
    " paid_seconds, used_credit, activate_from, deactivate_from, min_call_id, max_call_id) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsNNPPackageMinuteParts[0]) {
        StatsNNPPackageMinute &stats = it.second;

        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << stats.id << "',";
        query << "'" << stats.nnp_account_tariff_light_id << "',";
        query << "'" << stats.nnp_package_minute_id << "',";
        query << "'" << stats.used_seconds << "',";
        query << "'" << stats.paid_seconds << "',";
        query << "'" << stats.used_credit << "',";
        query << "'" << string_time(stats.activate_from, 8) << "',";
        query << "'" << string_time(stats.deactivate_from, 9) << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }

    if (i == 0) {
        query.str(std::string());
    }
}

void StatsNNPPackageMinuteManager::executeSaveQuery(BDb *dbCalls, stringstream &query) {
    string q = query.str();
    if (q.size() > 0) {
        dbCalls->exec(q);
    }
}

void StatsNNPPackageMinuteManager::createNewPartition() {
    realtimeStatsNNPPackageMinuteParts.push_back(map<int, StatsNNPPackageMinute>());
}

void StatsNNPPackageMinuteManager::removePartitionAfterSave() {
    realtimeStatsNNPPackageMinuteParts.erase(realtimeStatsNNPPackageMinuteParts.begin());
}

StatsNNPPackageMinute *StatsNNPPackageMinuteManager::createStatsNNPPackageMinute(time_t connect_time, Client *account,
                                                                                 StatsNNPPackageMinute *nnpPackageMinute) {

    time_t activate_dt;
    time_t deactivate_dt;

    ////////////////  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    activate_dt = get_tmonth(connect_time, account->timezone_offset);
//    deactivate_dt = get_tmonth_end(connect_time, account->timezone_offset);

    activate_dt = nnpPackageMinute->activate_from;
    deactivate_dt = nnpPackageMinute->deactivate_from;

    lastTrunkNNPPackageMinuteId += 1;

    StatsNNPPackageMinute &stats = statsNNPPackageMinute[lastTrunkNNPPackageMinuteId];
    stats.id = lastTrunkNNPPackageMinuteId;
    stats.nnp_package_minute_id = nnpPackageMinute->nnp_package_minute_id;
    stats.nnp_account_tariff_light_id = nnpPackageMinute->nnp_account_tariff_light_id;
    stats.activate_from = activate_dt;
    stats.deactivate_from = deactivate_dt;
    stats.used_seconds = 0;
    stats.used_credit = 0;
    stats.min_call_id = 0;
    stats.max_call_id = 0;

    statsByNNPPackageMinuteId[stats.id].push_front(stats.id);

    return &stats;
}

StatsNNPPackageMinute *StatsNNPPackageMinuteManager::updateStatsNNPPackageMinute(CallInfo *callInfo,
                                                                                 int statNNPPackageMinuteId) {

    auto itStatsNNPPackageMinute = statsNNPPackageMinute.find(statNNPPackageMinuteId);
    if (itStatsNNPPackageMinute == statsNNPPackageMinute.end()) {
        return nullptr;
    }

    StatsNNPPackageMinute &stats = itStatsNNPPackageMinute->second;

    stats.used_seconds += callInfo->call->billed_time;
    stats.used_credit += callInfo->call->cost;
    stats.min_call_id = stats.min_call_id == 0 ? callInfo->call->id : stats.min_call_id;
    stats.max_call_id = callInfo->call->id;

    return &stats;
}

void StatsNNPPackageMinuteManager::getChanges(map<int, StatsNNPPackageMinute> &changes) {
    for (int id : forSync) {
        changes[id] = statsNNPPackageMinute[id];
    }
    forSync.clear();
}

void StatsNNPPackageMinuteManager::addChanges(map<int, StatsNNPPackageMinute> &changes) {
    for (auto it : changes) {
        forSync.insert(it.first);
    }
}

size_t StatsNNPPackageMinuteManager::sync(BDb *db_main, BDb *db_calls) {

    BDbResult resMax = db_main->query(
            "SELECT max(max_call_id) FROM billing.stats_nnp_package_minute WHERE server_id='" +
            app().conf.str_instance_id +
            "'");
    long long int central_max_call_id = resMax.next() ? resMax.get_ll(0) : 0;

    BDbResult res = db_calls->query(
            " SELECT id, nnp_account_tariff_light_id, nnp_package_minute_id, used_seconds, used_credit, paid_seconds, activate_from, deactivate_from, min_call_id, max_call_id " \
        " from billing.stats_nnp_package_minute " \
        " where max_call_id > '" + lexical_cast<string>(central_max_call_id) + "'");

    stringstream query;
    query <<
    "INSERT INTO billing.stats_nnp_package_minute(server_id, id, nnp_account_tariff_light_id, nnp_package_minute_id, used_seconds, " \
    " used_credit, paid_seconds, activate_from, deactivate_from, min_call_id, max_call_id) VALUES\n";

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
        query << "'" << res.get(8) << "',";
        query << "'" << res.get(9) << "')";
        i++;
    }

    if (res.size() > 0) {
        try {
            db_main->exec(query.str());
        } catch (Exception &e) {
            e.addTrace("StatsNNPPackageMinuteManager::sync:");
            throw e;
        }
    }

    return res.size();
}

int StatsNNPPackageMinuteManager::getUsedSeconds(int nnp_account_tariff_light_id, int nnp_package_minute_id) {

    for (auto it = statsNNPPackageMinute.begin(); it != statsNNPPackageMinute.end(); it++) {
        if (it->second.nnp_account_tariff_light_id == nnp_account_tariff_light_id &&
            it->second.nnp_package_minute_id == nnp_package_minute_id)
            return it->second.used_seconds;
    }
    return 0;

}
