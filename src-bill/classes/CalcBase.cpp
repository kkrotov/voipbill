#include "CalcBase.h"
#include "AppBill.h"

#include "../threads/ThreadSelectGlobalCounters.h"

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

        calc_call(call);
    }
    if (call != 0) last_call_time = call->dt.time;
}

void CalcBase::calc_current(CurrentCallsObjList *list) {
    data.day = 0;

    counter_local.clear();

    lock_guard<mutex> lock(data_global->counter_rwlock);

    time_t current_time = time(NULL);

    pCallObj call = 0;
    for (int i = 0; i < list->count; i++) {
        call = list->get(i);

        call->len = current_time - call->dt.time;
        if (call->len < 0) call->len = 0;
        call->len_mcn = call->len;

        calc_call(call);
    }
}

void CalcBase::calc_call(pCallObj call) {
    prepare(call->dt);
    calc_item(call);
}

void CalcBase::calc_process_result(CurrentCallsObjList *list) {
    for (int i = 0; i < list->count; i++) {
        pCallObj call = list->get(i);
        calc_process_call(call);
    }
}

void CalcBase::calc_process_call(pCallObj call) {

    if (call->isCallFromAnotherInstance())
        return;

    // clients-operators or this region
    if (call->usage_id == 0 && (call->instance_id >= 200 || call->instance_id == app().conf.instance_id)) {
        call->kill_call_reason = KILL_REASON_UNKNOWN_NUMBER;
        return;
    }
    if (call->out == false) return;

    pClientObj client = data.client->find(call->client_id);
    if (client == 0) {
        call->kill_call_reason = KILL_REASON_UNKNOWN_CLIENT;
        return;
    }

    if (call->prefix_mcn[0] == 0) {
        call->kill_call_reason = KILL_REASON_UNKNOWN_MCN_PRICE;
        return;
    }

    ClientCounterObj &c0 = data.counter_client->get(call->client_id);
    ClientCounterObj &c2 = client_counter2->get(call->client_id);



    pGlobalCountersObj globalCounter =
            ThreadSelectGlobalCounters::getList()->find(call->client_id);

    double globalBalanceSum = 0, globalDaySum = 0, globalMonthSum = 0;
    if (globalCounter) {
        globalBalanceSum = globalCounter->sumBalance();
        globalDaySum = globalCounter->sumDay();
        globalMonthSum = globalCounter->sumMonth();
    }

    double spentBalanceSum = c0.sumBalance() + c2.sumBalance() + globalBalanceSum;
    double spentDaySum = c0.sumDay() + c2.sumDay() + globalDaySum;
    double spentMonthSum = c0.sumMonth() + c2.sumMonth() + globalMonthSum;

    if (call->isLocal()) {
        if (client->isConsumedCreditLimit(spentBalanceSum)
                && client->last_payed_month < get_tmonth()) {
            call->kill_call_reason = KILL_REASON_CREDIT_LIMIT;
            
            Log::notice(
                "KILL Reason " + string(call->usage_num) + " / " + string(call->phone_num) + " : Credit limit: <b>" + string_fmt("%.2f", client->balance + client->credit - c0.sumBalance() - c2.sumBalance() - globalBalanceSum) + "</b> = " +
                string_fmt("%.2f", client->balance) + " (balance) + " + string_fmt("%d", client->credit) + " (credit) - " + string_fmt("%.2f", c0.sumBalance()) + " (local) - " + string_fmt("%.2f", c2.sumBalance()) + " (current) - " + string_fmt("%.2f", globalBalanceSum) + " (global) <br/>\n"
            );

            return;
        }
    } else {
        if (client->disabled) {
            call->kill_call_reason = KILL_REASON_VOIP_DISABLED;
            return;
        }

        if (client->isConsumedCreditLimit(spentBalanceSum)) {
            call->kill_call_reason = KILL_REASON_CREDIT_LIMIT;
            
            Log::notice(
                "KILL Reason " + string(call->usage_num) + " / " + string(call->phone_num) + " : Credit limit: <b>" + string_fmt("%.2f", client->balance + client->credit - c0.sumBalance() - c2.sumBalance() - globalBalanceSum) + "</b> = " +
                string_fmt("%.2f", client->balance) + " (balance) + " + string_fmt("%d", client->credit) + " (credit) - " + string_fmt("%.2f", c0.sumBalance()) + " (local) - " + string_fmt("%.2f", c2.sumBalance()) + " (current) - " + string_fmt("%.2f", globalBalanceSum) + " (global) <br/>\n"
            );

            return;
        }
        if (client->isConsumedDailyLimit(spentDaySum)) {
            call->kill_call_reason = KILL_REASON_DAILY_LIMIT;

            Log::notice(
                "KILL Reason " + string(call->usage_num) + " / " + string(call->phone_num) + " : Daily limit: <b>" + string_fmt("%.2f", client->limit_d - c0.sumDay() - c2.sumDay() - globalDaySum) + "</b> = " +
                string_fmt("%d", client->limit_d) + " (limit_d) - " + string_fmt("%.2f", c0.sumDay()) + " (local) - " + string_fmt("%.2f", c2.sumDay()) + " (current) - " + string_fmt("%.2f", globalDaySum) + " (global) <br/>\n"
            );

            return;
        }
        if (client->isConsumedMonthlyLimit(spentMonthSum)) {           
            call->kill_call_reason = KILL_REASON_MONTHLY_LIMIT;
            
            Log::notice(
                "KILL Reason " + string(call->usage_num) + " / " + string(call->phone_num) + " : Monthly limit: <b>" + string_fmt("%.2f", client->limit_m - c0.sumMonth() - c2.sumMonth() - globalMonthSum) + "</b> = " +
                string_fmt("%d", client->limit_m) + " (limit_m) - " + string_fmt("%.2f", c0.sumMonth()) + " (local) - " + string_fmt("%.2f", c2.sumMonth()) + " (current) - " + string_fmt("%.2f", globalMonthSum) + " (global) <br/>\n"
            );

            return;
        }
    }

}

void CalcBase::save() {
    data_global->counter_append(counter_local);
}


