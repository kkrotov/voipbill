#include "CalcBase.h"
#include "App.h"

CalcBase::CalcBase() {
    last_call_time = 0;
    this->db = 0;
    this->data_global = DataLoader::instance();

    client_counter2 = counter_local.client;
}

CalcBase::~CalcBase() {
}

void CalcBase::setDb(BDb *db) {
    this->db = db;
}

void CalcBase::setDataLoader(DataLoader *data_loader) {
    this->data_global = data_loader;
}

void CalcBase::prepare(DT & dt) {

    if (data.day == dt.day) return;

    const bool countersAlreadyLocked = true;

    if (!data_global->get(dt, data, countersAlreadyLocked)) {


        if (!data_global->load(db, dt, data, countersAlreadyLocked)) {

            throw Exception("cant load data");

        }


    }

    client_counter2 = counter_local.client;
    fmin_counter2 = counter_local.fmin.get_or_load(db, dt.month, dt.month);

}

void CalcBase::calc(CallsObjList *list) {
    data.day = 0;

    counter_local.clear();

    lock_guard<mutex> lock(data_global->counter_rwlock);

    pCallObj call = 0;
    for (int i = 0; i < list->count; i++) {
        call = list->get(i);

        prepare(call->dt);

        calc_item(call);
    }
    if (call != 0) last_call_time = call->dt.time;
}

void CalcBase::calc_limit(CurrentCallsObjList *list) {
    data.day = 0;

    counter_local.clear();

    {
        lock_guard<mutex> lock(data_global->counter_rwlock);

        time_t current_time = time(NULL);

        for (int i = 0; i < list->count; i++) {
            pCallObj call = list->get(i);

            call->len = current_time - call->dt.time;
            if (call->len < 0) call->len = 0;
            call->len_mcn = call->len;

            prepare(call->dt);

            calc_item(call);
        }
    }

    for (int i = 0; i < list->count; i++) {
        pCallObj call = list->get(i);
        // clients-operators or this region
        if (call->usage_id == 0 && (call->instance_id >= 100 || call->instance_id == app.conf.instance_id)) {
            call->kill_call_reason = 1000;
            continue;
        }
        if (call->out == false) continue;

        pClientObj client = data.client->find(call->client_id);
        if (client == 0) {
            call->kill_call_reason = 1010;
            continue;
        }

        ClientCounterObj &c0 = data.counter_client->get(call->client_id);
        ClientCounterObj &c2 = client_counter2->get(call->client_id);

        if (call->isLocal()) {
            if (client->credit >= 0 && client->balance + client->credit - c0.sumBalance() - c2.sumBalance() < 0
                    && client->last_payed_month < get_tmonth()) {
                call->kill_call_reason = 3000;
                continue;
            }
        } else {
            if (client->disabled) {
                call->kill_call_reason = 2000;
                continue;
            }

            if (client->credit >= 0 && client->balance + client->credit - c0.sumBalance() - c2.sumBalance() < 0) {
                call->kill_call_reason = 3000;
                continue;
            }
            if (client->limit_d != 0 && client->limit_d - c0.sumDay() - c2.sumDay() < 0) {
                call->kill_call_reason = 3010;
                continue;
            }
            if (client->limit_m != 0 && client->limit_m - c0.sumMonth() - c2.sumMonth() < 0) {
                call->kill_call_reason = 3020;
                continue;
            }
        }




    }
}

void CalcBase::save() {
    data_global->counter_append(counter_local);
}


