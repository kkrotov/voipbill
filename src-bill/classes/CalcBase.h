#pragma once

#include "DataLoader.h"
#include "../lists/CallsObjList.h"

class CalcBase {
public:
    DataLoader *data_global;
    CounterLoader counter_local;

    shared_ptr<ClientCounter> client_counter2;
    shared_ptr<FminCounter> fmin_counter2;
protected:
    BDb *db;

    curr_data data;

    virtual void calc_item(pCallObj call) = 0;

public:
    time_t last_call_time;

    CalcBase();

    virtual ~CalcBase();

    void setDb(BDb *db);
    void setDataLoader(DataLoader *data_loader);


    void prepare(DT & dt);

    void calc(CallsObjList *list);
    void calc_call(pCallObj call);
    void calc_current(CurrentCallsObjList *list);
    void calc_process_result(CurrentCallsObjList *list);
    void calc_process_call(pCallObj call);

    void save();
};

