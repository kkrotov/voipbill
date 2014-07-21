#include "DataLoader.h"
#include "../../src/classes/Log.h"

DataLoader::DataLoader() {

}

bool DataLoader::get(DT & dt, curr_data & data, bool countersAlreadyLocked) {

    {
        unique_lock<mutex> lock(counter_rwlock, defer_lock);
        if (!countersAlreadyLocked)
            lock.lock();

        data.counter_fmin = counter_fmin.get(dt.month);
        data.counter_client = counter_client;
    }

    {
        lock_guard<mutex> lock(rwlock);

        data.client = client;
        data.dest = dest;
        data.oper = oper;
        data.pricelist = pricelist;
        data.usage = usage.get(dt.day);
        data.price = price.get(dt.day);
        data.network_prefix = network_prefix.get(dt.day);
    }

    return ready(dt, data);
}

bool DataLoader::load(BDb *db, DT & dt, curr_data & data, bool countersAlreadyLocked) {

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

    {
        lock_guard<mutex> lock(rwlock);
        try {
            data.client = client;
            data.dest = dest;
            data.oper = oper;
            data.pricelist = pricelist;
            data.usage = usage.get_or_load(db, dt.day);
            data.price = price.get_or_load(db, dt.day);
            data.network_prefix = network_prefix.get_or_load(db, dt.day);
        } catch (Exception &e) {
            Log::error(e.what());
            return false;
        }
    }

    return ready(dt, data);
}

inline bool DataLoader::ready(DT & dt, curr_data & data) {

    if (data.client == 0 || data.dest == 0 || data.oper == 0 || data.pricelist == 0 ||
            data.usage == 0 ||
            data.price == 0 ||
            data.network_prefix == 0 ||
            data.counter_fmin == 0 ||
            data.counter_client == 0
            ) {
        data.day = 0;
        return false;
    }

    data.day = dt.day;
    return true;
}

void DataLoader::counter_append(CounterLoader & loader2) {
    lock_guard<mutex> lock(counter_rwlock);

    counter_client->append(loader2.client.get());
    counter_fmin.append(loader2.fmin);
}

