#include "DataLoader.h"
#include "../../src/classes/Log.h"

DataLoader::DataLoader() {

}

bool DataLoader::get(DT &dt, curr_data &data, bool countersAlreadyLocked) {

    {
        unique_lock<mutex> lock(counter_rwlock, defer_lock);
        if (!countersAlreadyLocked)
            lock.lock();

        data.counter_fmin = counter_fmin.get(dt.month);
        data.counter_client = counter_client;
    }

    return ready(dt, data);
}

bool DataLoader::load(BDb *db, DT &dt, curr_data &data, bool countersAlreadyLocked) {

    {
        unique_lock<mutex> lock(counter_rwlock, defer_lock);
        if (!countersAlreadyLocked)
            lock.lock();

        try {
            data.counter_fmin = counter_fmin.get_or_load(db, dt.month, dt.month);
            data.counter_client = counter_client;
        } catch (Exception &e) {
            Log::error(e.what());
            return false;
        }
    }

    return ready(dt, data);
}

inline bool DataLoader::ready(DT &dt, curr_data &data) {

    if (
            data.counter_fmin == 0 ||
            data.counter_client == 0
            ) {
        data.day = 0;
        return false;
    }

    data.day = dt.day;
    return true;
}

void DataLoader::counter_append(CounterLoader &loader2) {
    lock_guard<mutex> lock(counter_rwlock);

    counter_client->append(loader2.client.get());
    counter_fmin.append(loader2.fmin);
}

