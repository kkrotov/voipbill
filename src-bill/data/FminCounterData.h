#pragma once

#include "BaseData.h"
#include "../lists/FminCounter.h"

class FminCounterData : public BaseData<FminCounter> {
public:
    long long int last_call_id;

    void init(FminCounter * item) {
        item->last_call_id = last_call_id;
    }

};