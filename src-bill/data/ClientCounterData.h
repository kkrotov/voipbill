#pragma once

#include "BaseData.h"
#include "../lists/ClientCounter.h"

class ClientCounterData : public BaseData<ClientCounter> {
public:
    long long int last_call_id;

    void init(ClientCounter * item) {
        item->last_call_id = last_call_id;
    }

};