#include "CalcBase.h"
#include "App.h"

CalcBase::CalcBase(){
    last_call_time = 0;
    this->db = 0;
    this->data_global = DataLoader::instance();
}

CalcBase::~CalcBase(){
}

void CalcBase::setDb(BDb *db){
    this->db = db;
}

void CalcBase::prepare(DT & dt){

    if (data.day == dt.day) return;

    data_global->counter_rwlock.unlock();

    if (!data_global->get(dt, data)){


        if (!data_global->load(db, dt, data)){

            data_global->counter_rwlock.lock(); // lockForRead();
            throw DbException("cant load data");

        }


    }

    data_global->counter_rwlock.lock(); // lockForRead();

    client_counter2 =   counter_local.client;
    fmin_counter2 =     counter_local.fmin.get_or_load(db, dt.month, dt.month);

}

void CalcBase::calc(CallsObjList *list){
    data.day = 0;

    counter_local.clear();

    //QReadLocker counter_read_locker(&data_global->counter_rwlock);
    data_global->counter_rwlock.lock();
    try
    {
        pCallObj call = 0;
        for(int i = 0; i < list->count; i++) {
            call = list->get(i);

            prepare( call->dt );

            calc_item(call);
        }
        if (call != 0) last_call_time = call->dt.time;
    }catch(exception &e)
    {
        data_global->counter_rwlock.unlock();
        throw e;
    }
    data_global->counter_rwlock.unlock();
    //counter_read_locker.unlock();
}

void CalcBase::calc_limit(CurrentCallsObjList *list){
    data.day = 0;

    counter_local.clear();

    //QReadLocker counter_read_locker(&data_global->counter_rwlock);
    data_global->counter_rwlock.lock();
    try
    {
        time_t current_time = time(NULL);

        for(int i = 0; i < list->count; i++) {
            pCallObj call = list->get(i);

            call->len = current_time - call->dt.time;
            if (call->len < 0) call->len = 0;

            prepare( call->dt );

            calc_item(call);
        }
    }catch(exception &e)
    {
        data_global->counter_rwlock.unlock();
        throw e;
    }
    data_global->counter_rwlock.unlock();
    //counter_read_locker.unlock();

    for(int i = 0; i < list->count; i++) {
        pCallObj call = list->get(i);
        // clients-operators or this region
        if (call->usage_id == 0 && (call->region >= 100 || call->region == app.conf.geo_region)){
            call->kill_call_reason = 1000;
            continue;
        }
        if (call->out == false) continue;

        pClientObj cl = data.client->find(call->client_id);
        if (cl == 0) continue;
        if (cl->limit_m == 0 && cl->limit_d == 0 && cl->credit < 0 && !cl->disabled) continue;

        if (cl->disabled)
        {
            call->kill_call_reason = 2000;
            continue;
        }


        ClientCounterObj &c0 = data.counter_client->get(call->client_id);
        ClientCounterObj &c2 = client_counter2->get(call->client_id);

        if (cl->credit  >= 0 && cl->balance + cl->credit - c0.sumBalance() - c2.sumBalance() < 0)
        {
            call->kill_call_reason = 3000;
            continue;
        }
        if (cl->limit_d != 0 && cl->limit_d - c0.sumDay() - c2.sumDay() < 0)
        {
            call->kill_call_reason = 3010;
            continue;
        }
        if (cl->limit_m != 0 && cl->limit_m - c0.sumMonth() - c2.sumMonth() < 0)
        {
            call->kill_call_reason = 3020;
            continue;
        }
    }
}

void CalcBase::save(){
    data_global->counter_append(counter_local);
}


