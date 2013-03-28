#pragma once

#include "DataLoader.h"
#include "../lists/CallsObjList.h"
#include "../lists/CurrentCallsObjList.h"

class CalcBase
{
public:
    DataLoader *data_global;
    CounterLoader counter_local;

    shared_ptr<ClientCounter> client_counter2;
    shared_ptr<FminCounter> fmin_counter2;
protected:
    BDb *db;

    time_t last_call_time;

    curr_data data;

	virtual void calc_item(pCallObj call) = 0;

public:
    CalcBase();

    virtual ~CalcBase();

    void setDb(BDb *db);


    void prepare(DT & dt);

    void calc(CallsObjList *list);

    void calc_limit(CurrentCallsObjList *list);

    void save();
};

