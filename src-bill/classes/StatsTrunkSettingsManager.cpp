#include "StatsTrunkSettingsManager.h"
#include "Exception.h"
#include "AppBill.h"

StatsTrunkSettingsManager::StatsTrunkSettingsManager() {
    realtimeStatsTrunkSettingsParts.push_back(map<int, StatsTrunkSettings>());
}

void StatsTrunkSettingsManager::load(BDb * db, time_t lastSaveCallTime) {
    forSync.clear();
    statsTrunkSettings.clear();
    statsByTrunkSettingsId.clear();
    realtimeStatsTrunkSettingsParts.clear();
    realtimeStatsTrunkSettingsParts.push_back(map<int, StatsTrunkSettings>());

    BDbResult resId = db->query("   select max(id) from billing.stats_trunk_settings p ");
    lastTrunkSettingsStatId = resId.next() ? resId.get_i(0) : 0;

    time_t filterFrom = lastSaveCallTime;
    if (filterFrom > 0) {
        filterFrom = filterFrom - 86400 * 60;
    }

    BDbResult res = db->query(
            "   select id, trunk_settings_id, used_seconds, used_credit, extract(epoch from activation_dt), extract(epoch from expire_dt), min_call_id, max_call_id " \
            "   from billing.stats_trunk_settings " \
            "   where activation_dt >= '" + string_time(filterFrom, 7) + "'" \
            "   order by trunk_settings_id asc, activation_dt asc "
    );
    while (res.next()) {
        StatsTrunkSettings &stats = statsTrunkSettings[res.get_i(0)];
        stats.id = res.get_i(0);
        stats.trunk_settings_id = res.get_i(1);
        stats.used_seconds = res.get_i(2);
        stats.used_credit = res.get_d(3);
        stats.activation_dt = res.get_ll(4);
        stats.expire_dt = res.get_ll(5);
        stats.min_call_id = res.get_ll(6);
        stats.max_call_id = res.get_ll(7);

        statsByTrunkSettingsId[stats.trunk_settings_id].push_front(stats.id);
    }

    loaded = true;
}

void StatsTrunkSettingsManager::recalc(BDb * db, long long int storedLastId) {
    string strStoredLastId = lexical_cast<string>(storedLastId);

    db->exec("DELETE FROM billing.stats_trunk_settings WHERE min_call_id > " + strStoredLastId);

    BDbResult res = db->query("select min(min_call_id) from billing.stats_trunk_settings where max_call_id >= " + strStoredLastId);
    int minCallId = res.next() ? res.get_i(0) : 0;

    if (minCallId > 0) {
        string strMinCallId = lexical_cast<string>(minCallId);

        BDbTransaction trans(db);

        db->exec(    "    update billing.stats_trunk_settings set min_call_id = 0 where max_call_id >= " + strStoredLastId);

        db->exec(    "    update billing.stats_trunk_settings p                             " \
                     "    set                                                               " \
                     "        min_call_id = s.min_call_id,                                  " \
                     "        max_call_id = s.max_call_id,                                  " \
                     "        used_seconds = coalesce(s.used_seconds, 0),                   " \
                     "        used_credit = coalesce(s.used_credit, 0)                      " \
                     "    from (                                                            " \
                     "        select  trunk_settings_stats_id as id,                        " \
                     "                sum(billed_time) as used_seconds,                     " \
                     "                sum(cost) as used_credit,                             " \
                     "                max(id) as min_call_id, max(id) as max_call_id        " \
                     "        from calls_raw.calls_raw                                      " \
                     "        where id >= " + strMinCallId + "                              " \
                     "        group by trunk_settings_stats_id                              " \
                     "    ) s                                                               " \
                     "    where p.max_call_id >= " + strStoredLastId + " and p.id = s.id    ");

        db->exec(    "    delete from billing.stats_trunk_settings where min_call_id = 0    ");

        trans.commit();
    }
}


StatsTrunkSettings * StatsTrunkSettingsManager::getCurrent(time_t connect_time, Client * account, ServiceTrunkSettings * trunkSettings) {
    auto itByTrunkSettingsId = statsByTrunkSettingsId.find(trunkSettings->id);
    if (itByTrunkSettingsId != statsByTrunkSettingsId.end()) {
        for (int trunkSettingsId : itByTrunkSettingsId->second) {
            auto itStatsTrunkSettings = statsTrunkSettings.find(trunkSettingsId);
            if (itStatsTrunkSettings == statsTrunkSettings.end()) continue;

            StatsTrunkSettings &stats = itStatsTrunkSettings->second;

            if (stats.activation_dt > connect_time) continue;
            if (stats.expire_dt < connect_time) continue;

            return &stats;
        }
    }

    StatsTrunkSettings * stats = createStatsTrunkSettings(connect_time, account, trunkSettings);

    size_t parts = realtimeStatsTrunkSettingsParts.size();
    map<int, StatsTrunkSettings> &realtimeStatsTrunkSettings = realtimeStatsTrunkSettingsParts.at(parts - 1);
    StatsTrunkSettings &stats2 = realtimeStatsTrunkSettings[stats->id];
    memcpy(&stats2, stats, sizeof(StatsTrunkSettings));
    forSync.insert(stats->id);

    return stats;
}

void StatsTrunkSettingsManager::add(CallInfo * callInfo) {

    if (callInfo->call->trunk_settings_stats_id == 0) {
        return;
    }

    StatsTrunkSettings * stats = updateStatsTrunkSettings(callInfo, callInfo->call->trunk_settings_stats_id);
    if (stats != nullptr) {
        size_t parts = realtimeStatsTrunkSettingsParts.size();
        map<int, StatsTrunkSettings> &realtimeStatsTrunkSettings = realtimeStatsTrunkSettingsParts.at(parts - 1);
        StatsTrunkSettings &stats2 = realtimeStatsTrunkSettings[stats->id];
        memcpy(&stats2, stats, sizeof(StatsTrunkSettings));

        forSync.insert(stats->id);
    }
}

size_t StatsTrunkSettingsManager::size() {
    return statsTrunkSettings.size();
}

void StatsTrunkSettingsManager::prepareSaveQuery(stringstream &query) {

    if (realtimeStatsTrunkSettingsParts[0].size() == 0) {
        return;
    }

    query << "INSERT INTO billing.stats_trunk_settings(id, trunk_settings_id, used_seconds, used_credit, activation_dt, expire_dt, min_call_id, max_call_id) VALUES\n";
    int i = 0;
    for (auto it : realtimeStatsTrunkSettingsParts[0]) {
        StatsTrunkSettings &stats = it.second;

        if (stats.min_call_id == 0 || stats.max_call_id == 0) {
            continue;
        }

        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << stats.id << "',";
        query << "'" << stats.trunk_settings_id << "',";
        query << "'" << stats.used_seconds << "',";
        query << "'" << stats.used_credit << "',";
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

void StatsTrunkSettingsManager::executeSaveQuery(BDb * dbCalls, stringstream &query) {
    string q = query.str();
    if (q.size() > 0) {

        dbCalls->exec(q);

    }
}

void StatsTrunkSettingsManager::createNewPartition() {
    realtimeStatsTrunkSettingsParts.push_back(map<int, StatsTrunkSettings>());
}

void StatsTrunkSettingsManager::removePartitionAfterSave() {
    realtimeStatsTrunkSettingsParts.erase(realtimeStatsTrunkSettingsParts.begin());
}

StatsTrunkSettings * StatsTrunkSettingsManager::createStatsTrunkSettings(time_t connect_time, Client * account, ServiceTrunkSettings * trunkSettings) {

    time_t activation_dt;
    time_t expire_dt;

    activation_dt = get_tmonth(connect_time, account->timezone_offset);
    expire_dt = get_tmonth_end(connect_time, account->timezone_offset);

    lastTrunkSettingsStatId += 1;

    StatsTrunkSettings &stats = statsTrunkSettings[lastTrunkSettingsStatId];
    stats.id = lastTrunkSettingsStatId;
    stats.trunk_settings_id = trunkSettings->id;
    stats.activation_dt = activation_dt;
    stats.expire_dt = expire_dt;
    stats.used_seconds = 0;
    stats.used_credit = 0;
    stats.min_call_id = 0;
    stats.max_call_id = 0;

    statsByTrunkSettingsId[stats.trunk_settings_id].push_front(stats.id);

    return &stats;
}

StatsTrunkSettings * StatsTrunkSettingsManager::updateStatsTrunkSettings(CallInfo *callInfo, int statTrunkSettingsId) {

    auto itStatsTrunkSettings = statsTrunkSettings.find(statTrunkSettingsId);
    if (itStatsTrunkSettings == statsTrunkSettings.end()) {
        return nullptr;
    }

    StatsTrunkSettings &stats = itStatsTrunkSettings->second;

    stats.used_seconds += callInfo->call->billed_time;
    stats.used_credit += callInfo->call->cost;
    stats.min_call_id = stats.min_call_id == 0 ? callInfo->call->id : stats.min_call_id;
    stats.max_call_id = callInfo->call->id;

    return &stats;
}

void StatsTrunkSettingsManager::getChanges(map<int, StatsTrunkSettings> &changes) {
    for (int id : forSync) {
        changes[id] = statsTrunkSettings[id];
    }
    forSync.clear();
}

void StatsTrunkSettingsManager::addChanges(map<int, StatsTrunkSettings> &changes) {
    for (auto it : changes) {
        forSync.insert(it.first);
    }
}

size_t StatsTrunkSettingsManager::sync(BDb * db_main, BDb * db_calls) {

    BDbResult resMax = db_main->query("SELECT max(max_call_id) FROM billing.stats_trunk_settings WHERE server_id='" + app().conf.str_instance_id +"'");
    long long int central_max_call_id = resMax.next() ? resMax.get_ll(0) : 0;

    BDbResult res = db_calls->query(
            " SELECT id, trunk_settings_id, used_seconds, used_credit, activation_dt, expire_dt, min_call_id, max_call_id " \
        " from billing.stats_trunk_settings " \
        " where max_call_id > '" + lexical_cast<string>(central_max_call_id) + "'");

    stringstream query;
    query << "INSERT INTO billing.stats_trunk_settings(server_id, id, trunk_settings_id, used_seconds, used_credit, activation_dt, expire_dt, min_call_id, max_call_id) VALUES\n";

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
        query << "'" << res.get(7) << "')";
        i++;
    }

    if (res.size() > 0) {
        try {
            db_main->exec(query.str());
        } catch (Exception &e) {
            e.addTrace("StatsTrunkSettingsManager::sync:");
            throw e;
        }
    }

    return res.size();
}