#include "DataLoader.h"
#include "Log.h"

DataLoader::DataLoader(){

}

bool DataLoader::get(DT & dt, curr_data & data){
    counter_rwlock.lock(); // lockForRead();
    data.counter_fmin =     counter_fmin.get(dt.month);
    data.counter_client =   counter_client;
    counter_rwlock.unlock();

    rwlock.lock(); // lockForRead();
    data.client =   client;
    data.dest =     dest;
    data.oper =     oper;
    data.usage =    usage.get(dt.day);
    data.price =    price.get(dt.day);
    rwlock.unlock();

    if (    data.client == 0 || data.dest == 0 || data.oper == 0 ||
            data.usage == 0 ||
            data.price == 0 ||
            data.counter_fmin == 0 ||
            data.counter_client == 0
       )
    {
        data.day = 0;
        return false;
    }
    data.day = dt.day;
    return true;
}
bool DataLoader::load(BDb *db, DT & dt, curr_data & data){
    counter_rwlock.lock(); // lockForWrite();
    try{
        data.counter_fmin =     counter_fmin.get_or_load(db, dt.month, dt.month);
        data.counter_client =   counter_client;
    }catch( Exception &e ){
        counter_rwlock.unlock();
        Log::error(e.what());
        return false;
    }
    counter_rwlock.unlock();


    rwlock.lock(); // .lockForWrite();
    try{
        data.client =   client;
        data.dest =     dest;
        data.oper =     oper;
        data.usage =    usage.get_or_load(db, dt.day);
        data.price =    price.get_or_load(db, dt.day);
    }catch( Exception &e ){
        rwlock.unlock();
        Log::error(e.what());
        return false;
    }
    rwlock.unlock();

    return get(dt, data);
}

void DataLoader::counter_append(CounterLoader & loader2){
    counter_rwlock.lock(); // .lockForWrite();

    counter_client->append(loader2.client.get());
    counter_fmin.append(loader2.fmin);

    counter_rwlock.unlock();
}

