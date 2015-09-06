#include "CallsManager.h"

CallsManager::CallsManager() {
    realtimeLastId = -1;
    realtimeLastTime = 0;
    realtimeLastId = 0;
    tmpLastId = -1;
    tmpLastTime = 0;
    tmpCounter = 0;
    storedLastId = -1;
    storedLastTime = 0;
    storedCounter = 0;
}

bool CallsManager::ready() {
    lock_guard<Spinlock> guard(lock);

    return realtimeLastId >= 0 && storedLastId >= 0;
}

void CallsManager::add(Call &origCall, Call &termCall) {
    lock_guard<Spinlock> guard(lock);

    realtimeQueue.push_back(origCall);
    realtimeQueue.push_back(termCall);

    realtimeLastId = termCall.id;
    realtimeLastTime = termCall.connect_time;
    realtimeCounter += 2;
}

void CallsManager::get(vector<Call> &calls, size_t maxCount) {
    lock_guard<Spinlock> guard(lock);

    maxCount = realtimeQueue.size() < maxCount ? realtimeQueue.size() : maxCount;
    calls.reserve(maxCount);

    while (realtimeQueue.size() > 0 && calls.size() < maxCount) {
        calls.push_back(realtimeQueue.front());
        realtimeQueue.pop_front();
    }
}

void CallsManager::revert(vector<Call> &calls) {
    lock_guard<Spinlock> guard(lock);

    for (auto it = calls.rbegin(); it!= calls.rend(); ++it) {
        realtimeQueue.push_front(*it);
    }
}

size_t CallsManager::size() {
    lock_guard<Spinlock> guard(lock);

    return realtimeQueue.size();
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

    if (tmpQueue.size() == 0) {
        return;
    }

    map<time_t, stringstream> queryPerMonth;

    for (Call &call : tmpQueue) {

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

void CallsManager::moveRealtimeToTemp() {
    lock_guard<Spinlock> guard(lock);

    while (realtimeQueue.size() > 0) {
        tmpQueue.push_back(realtimeQueue.front());
        realtimeQueue.pop_front();
    }

    tmpLastId = realtimeLastId;
    tmpLastTime = realtimeLastTime;
    tmpCounter += realtimeCounter;

    realtimeCounter = 0;
}

void CallsManager::moveTempToStored() {
    lock_guard<Spinlock> guard(lock);

    storedLastId = tmpLastId;
    storedLastTime = tmpLastTime;
    storedCounter += tmpCounter;

    tmpCounter = 0;
    tmpQueue.clear();
}

long long int CallsManager::getLastId() {
    lock_guard<Spinlock> guard(lock);

    return realtimeLastId;
}

time_t CallsManager::getLastTime() {
    lock_guard<Spinlock> guard(lock);

    return realtimeLastTime;
}

size_t CallsManager::getCounter() {
    lock_guard<Spinlock> guard(lock);

    return realtimeCounter;
}

void CallsManager::setLastId(long long int lastId) {
    this->realtimeLastId = lastId;
}

void CallsManager::setLastTime(time_t lastTime) {
    this->realtimeLastTime = lastTime;
}

long long int CallsManager::getStoredLastId() {
    lock_guard<Spinlock> guard(lock);

    return storedLastId;
}

time_t CallsManager::getStoredLastTime() {
    lock_guard<Spinlock> guard(lock);

    return storedLastTime;
}

size_t CallsManager::getStoredCounter() {
    lock_guard<Spinlock> guard(lock);

    return storedCounter;
}

void CallsManager::setStoredLastId(long long int storedLastId) {
    this->storedLastId = storedLastId;
}

void CallsManager::setStoredLastTime(time_t storedLastTime) {
    this->storedLastTime = storedLastTime;
}

void CallsManager::incStoredCounter(size_t count) {
    storedCounter += count;
}