#pragma once

#include <mutex>

#include "../lists/ServerList.h"
#include "../lists/ClientList.h"
#include "../lists/DestObjList.h"
#include "../lists/OperatorList.h"
#include "../lists/PricelistList.h"
#include "../lists/UsageObjList.h"
#include "../lists/PriceObjList.h"
#include "../lists/NetworkPrefixList.h"
#include "../lists/ClientCounter.h"
#include "../lists/FminCounter.h"


#include "../../src/classes/Loader.h"
#include "../classes/CounterLoader.h"

struct curr_data {
    time_t day;

    shared_ptr<ServerList> server;
    shared_ptr<ClientList> client;
    shared_ptr<DestObjList> dest;
    shared_ptr<OperatorList> oper;
    shared_ptr<PricelistList> pricelist;
    shared_ptr<UsageObjList> usage;
    shared_ptr<PriceObjList> price;
    shared_ptr<NetworkPrefixList> network_prefix;

    shared_ptr<ClientCounter> counter_client;
    shared_ptr<FminCounter> counter_fmin;
};

class DataLoader {
public:
    mutex rwlock;

    shared_ptr<ServerList> server;
    shared_ptr<ClientList> client;
    shared_ptr<DestObjList> dest;
    shared_ptr<OperatorList> oper;
    shared_ptr<PricelistList> pricelist;

    Loader<UsageObjList> usage;
    Loader<PriceObjList> price;
    Loader<NetworkPrefixList> network_prefix;

    mutex counter_rwlock;
    shared_ptr<ClientCounter> counter_client;
    Loader<FminCounter> counter_fmin;
    
    volatile time_t usageReloadTimestamp;

    DataLoader();

    static DataLoader * instance() {
        static DataLoader inst;
        return &inst;
    }

    bool get(DT & dt, curr_data & data, bool countersAlreadyLocked = false);

    bool load(BDb *db, DT & dt, curr_data & data, bool countersAlreadyLocked = false);

    bool ready(DT & dt, curr_data & data);

    void counter_append(CounterLoader & loader2);

};

