#pragma once

#include "../../src/classes/BDb.h"
#include "../../src/common.h"
#include "../models/Cdr.h"
#include "../models/Call.h"

class DataBillingContainer {
public:
    queue<Call> cdrs;
    queue<Call> calls;

    static DataBillingContainer * instance() {
        static DataBillingContainer inst;
        return &inst;
    }

    void loadAll(BDb * db) {
    }
};