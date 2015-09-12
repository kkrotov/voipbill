#include "CallsManager.h"

CallsManager::CallsManager() {
    realtimeLastId = -1;
    realtimeLastTime = 0;
    realtimeLastId = 0;
    storedLastId = -1;
    storedLastTime = 0;
    storedCounter = 0;

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


void calls_insert_row(Call * call, stringstream &q) {

    q << "(";
    q << "'" << call->id << "',";
    q << (call->orig ? "true" : "false") << ",";
    q << "'" << call->peer_id << "',";
    q << "'" << call->cdr_id << "',";
    q << "'" << string_time(call->connect_time) << "',";
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
    if (call->service_package_limit_id != 0) {
        q << "'" << call->service_package_limit_id << "',";
    } else {
        q << "NULL,";
    }
    q << "'" << call->package_time << "',";
    q << "'" << call->package_credit << "',";
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
    if (call->operator_id != 0) {
        q << "'" << call->operator_id << "',";
    } else {
        q << "NULL,";
    }
    q << (call->mob ? "true" : "false") << ",";
    q << (call->geo_mob ? "true" : "false");
    q << ")\n";
}

void CallsManager::save(BDb * dbCalls) {

    size_t parts = realtimeCallsParts.size();
    if (parts < 2) {
        return;
    }

    map<time_t, stringstream> queryPerMonth;

    for (Call &call : realtimeCallsParts[0]) {

        if (queryPerMonth.find(call.dt.month) == queryPerMonth.end()) {
            char buff[20];
            struct tm timeinfo;
            gmtime_r(&call.dt.month, &timeinfo);
            strftime(buff, 20, "%Y%m", &timeinfo);

            stringstream &q = queryPerMonth[call.dt.month];
            q << "INSERT INTO calls_raw.calls_raw_" + string(buff) + "(" \
                    "id,orig,peer_id,cdr_id,connect_time,trunk_id,account_id,trunk_service_id,number_service_id," \
                    "src_number,dst_number,billed_time,rate,cost,tax_cost,interconnect_rate,interconnect_cost," \
                    "service_package_id,service_package_limit_id,package_time,package_credit," \
                    "destination_id,pricelist_id,prefix,geo_id,geo_operator_id,operator_id,mob,geo_mob" \
                 ")VALUES\n";

            calls_insert_row(&call, q);

        } else {

            stringstream &q = queryPerMonth[call.dt.month];
            q << ",";

            calls_insert_row(&call, q);
        }

    }

    for (auto &it : queryPerMonth) {
        dbCalls->exec(it.second.str());
    }
}

void CallsManager::createNewPartition() {
    realtimeCallsParts.push_back(vector<Call>());
}

void CallsManager::afterSave() {
    size_t parts = realtimeCallsParts.size();
    if (parts > 1) {
        if (parts == 2) {
            realtimeCallsParts.push_back(vector<Call>());
        }
        realtimeCallsParts.erase(realtimeCallsParts.begin());
    }
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