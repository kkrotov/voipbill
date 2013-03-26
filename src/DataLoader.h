#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>
using boost::interprocess::interprocess_mutex;

#include "lists/UsageRawList.h"
#include "lists/PhoneList.h"
#include "lists/ClientObjList.h"
#include "lists/DestObjList.h"
#include "lists/OperatorList.h"
#include "lists/UsageObjList.h"
#include "lists/PriceObjList.h"
#include "lists/ClientCounter.h"
#include "lists/FminCounter.h"


#include "Loader.h"
#include "CounterLoader.h"

struct curr_data{
    time_t day;

    shared_ptr<ClientObjList> client;
    shared_ptr<DestObjList> dest;
    shared_ptr<OperatorList> oper;
    shared_ptr<UsageObjList> usage;
    shared_ptr<PriceObjList> price;

    shared_ptr<ClientCounter> counter_client;
    shared_ptr<FminCounter> counter_fmin;
};

class DataLoader
{
public:
    interprocess_mutex rwlock;
    Loader<UsageObjList> usage;
    Loader<PriceObjList> price;

    //shared_ptr<UsageRawList> usage_raw;
    shared_ptr<ClientObjList> client;
    shared_ptr<DestObjList> dest;
    shared_ptr<OperatorList> oper;
    //shared_ptr<PhoneList> phones;



    interprocess_mutex counter_rwlock;
    shared_ptr<ClientCounter> counter_client;
    Loader<FminCounter> counter_fmin;

    DataLoader();

    static DataLoader * instance()
    {
        static DataLoader inst;
        return &inst;
    }

    bool get(DT & dt, curr_data & data);

    bool load(BDb *db, DT & dt, curr_data & data);

    void counter_append(CounterLoader & loader2);

};

