#include "CallsManager.h"

CallsManager::CallsManager() {
    clear();
}

void CallsManager::clear()
{
    realtimeLastId = -1;
    realtimeLastTime = 0;
    storedLastId = -1;
    storedLastTime = 0;

    realtimeCounter = 0;
    storedCounter = 0;

    realtimeCallsParts.clear();
    realtimeCallsParts.push_back(vector<Call>());
}

bool CallsManager::ready() {
    return realtimeLastId >= 0 && storedLastId >= 0;
}


void CallsManager::add(Call &call) {
    size_t parts = realtimeCallsParts.size();
    vector<Call>&realtimeCalls = realtimeCallsParts.at(parts - 1);

    realtimeCalls.push_back(call);

    realtimeLastId = call.id;
    realtimeLastTime = call.connect_time;
    realtimeCounter += 1;
}

void CallsManager::add(Call &origCall, Call &termCall) {
    size_t parts = realtimeCallsParts.size();
    vector<Call> &realtimeCalls = realtimeCallsParts.at(parts - 1);

    realtimeCalls.push_back(origCall);
    realtimeCalls.push_back(termCall);

    realtimeLastId = termCall.id;
    realtimeLastTime = termCall.connect_time;
    realtimeCounter += 2;
}

size_t CallsManager::getQueueSize() {
    size_t size = 0;

    for (vector<Call> &realtimeCalls : realtimeCallsParts) {
        size += realtimeCalls.size();
    }

    return size;
}


void CallsManager::calls_insert_row(Call * call, stringstream &q) {

    q << "(";
    q << "'" << call->id << "',";
    q << (call->orig ? "true" : "false") << ",";
    q << (call->our ? "true" : "false") << ",";
    q << "'" << call->peer_id << "',";
    q << "'" << call->cdr_id << "',";
    q << "'" << string_time(call->connect_time, 2) << "',";
    if (call->trunk_id != 0) {
        q << "'" << call->trunk_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->account_id != 0) {
        q << "'" << call->account_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->trunk_service_id != 0) {
        q << "'" << call->trunk_service_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->number_service_id != 0) {
        q << "'" << call->number_service_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->src_number != 0) {
        q << "'" << call->src_number << "',";
    } else {
        q << "NULL,";
    }
    if (call->dst_number != 0) {
        q << "'" << call->dst_number << "',";
    } else {
        q << "NULL,";
    }
    q << "'" << call->billed_time << "',";
    q << "'" << call->rate << "',";
    q << "'" << call->cost << "',";
    q << "'" << call->tax_cost << "',";
    q << "'" << call->interconnect_rate << "',";
    q << "'" << call->interconnect_cost << "',";
    if (call->service_package_id != 0) {
        q << "'" << call->service_package_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->service_package_stats_id != 0) {
        q << "'" << call->service_package_stats_id << "',";
    } else {
        q << "NULL,";
    }
    q << "'" << call->package_time << "',";
    q << "'" << call->package_credit << "',";
    if (call->trunk_settings_stats_id != 0) {
        q << "'" << call->trunk_settings_stats_id << "',";
    } else {
        q << "NULL,";
    }
    q << "'" << call->destination_id << "',";
    if (call->pricelist_id != 0) {
        q << "'" << call->pricelist_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->prefix != 0) {
        q << "'" << call->prefix << "',";
    } else {
        q << "NULL,";
    }
    if (call->geo_id != 0) {
        q << "'" << call->geo_id << "',";
    } else {
        q << "NULL,";
    }
    if (call->geo_operator_id != 0) {
        q << "'" << call->geo_operator_id << "',";
    } else {
        q << "NULL,";
    }
    q << (call->mob ? "true" : "false") << ",";
    q << (call->geo_mob ? "true" : "false");
    q << ")\n";
}

void CallsManager::prepareSaveQueries(map<time_t, stringstream> &queryPerMonth) {

    if (realtimeCallsParts[0].size() == 0) {
        return;
    }

    for (Call &call : realtimeCallsParts[0]) {

        struct tm ttt;
        gmtime_r(&call.connect_time, &ttt);
        time_t dt_day = call.connect_time - ttt.tm_hour * 3600 - ttt.tm_min * 60 - ttt.tm_sec;
        time_t dt_month = dt_day - (ttt.tm_mday - 1)*86400;

        if (queryPerMonth.find(dt_month) == queryPerMonth.end()) {
            char buff[20];
            struct tm timeinfo;
            gmtime_r(&dt_month, &timeinfo);
            strftime(buff, 20, "%Y%m", &timeinfo);

            stringstream &q = queryPerMonth[dt_month];
            q << "INSERT INTO calls_raw.calls_raw_" + string(buff) + "(" \
                    "id,orig,our,peer_id,cdr_id,connect_time,trunk_id,account_id,trunk_service_id,number_service_id," \
                    "src_number,dst_number,billed_time,rate,cost,tax_cost,interconnect_rate,interconnect_cost," \
                    "service_package_id,service_package_stats_id,package_time,package_credit,trunk_settings_stats_id," \
                    "destination_id,pricelist_id,prefix,geo_id,geo_operator_id,mob,geo_mob" \
                 ")VALUES\n";

            calls_insert_row(&call, q);

        } else {

            stringstream &q = queryPerMonth[dt_month];
            q << ",";

            calls_insert_row(&call, q);
        }

    }
}

void CallsManager::executeSaveQueries(BDb * dbCalls, map<time_t, stringstream> &queryPerMonth) {

    for (auto &it : queryPerMonth) {
        dbCalls->exec(it.second.str());
    }

}

bool CallsManager::isNeedSave() {
    size_t parts = realtimeCallsParts.size();
    size_t firstPartSize = realtimeCallsParts.at(0).size();
    return firstPartSize > 0 || parts > 1;
}

bool CallsManager::isNeedCreatePartitionBeforeSave() {
    size_t parts = realtimeCallsParts.size();
    return parts == 1;
}

bool CallsManager::isNeedCreatePartitionAfterAdd() {
    size_t parts = realtimeCallsParts.size();
    size_t lastPartSize = realtimeCallsParts.at(parts - 1).size();
    return lastPartSize >= CALLS_PARTITION_SIZE;
}

void CallsManager::createNewPartition() {
    realtimeCallsParts.push_back(vector<Call>());
}

void CallsManager::removePartitionAfterSave() {
    vector<Call> &realtimeCalls = realtimeCallsParts.at(0);
    size_t realtimeCallsSize = realtimeCalls.size();
    if (realtimeCallsSize > 0) {
        storedCounter += realtimeCallsSize;
        storedLastId = realtimeCalls.at(realtimeCallsSize - 1).id;
        storedLastTime = realtimeCalls.at(realtimeCallsSize - 1).connect_time;
    }
    realtimeCallsParts.erase(realtimeCallsParts.begin());
}

long long int CallsManager::getLastId() {
    return realtimeLastId;
}

time_t CallsManager::getLastTime() {
    return realtimeLastTime;
}

size_t CallsManager::getCounter() {
    return realtimeCounter;
}

void CallsManager::setLastId(long long int lastId) {
    realtimeLastId = lastId;
}

void CallsManager::setLastTime(time_t lastTime) {
    realtimeLastTime = lastTime;
}

long long int CallsManager::getStoredLastId() {
    return storedLastId;
}

time_t CallsManager::getStoredLastTime() {
    return storedLastTime;
}

size_t CallsManager::getStoredCounter() {
    return storedCounter;
}

void CallsManager::setStoredLastId(long long int lastId) {
    storedLastId = lastId;
}

void CallsManager::setStoredLastTime(time_t lastTime) {
    storedLastTime = lastTime;
}

size_t CallsManager::getLastRealtimePartSize() {
    size_t parts = realtimeCallsParts.size();
    return realtimeCallsParts.at(parts - 1).size();
}
