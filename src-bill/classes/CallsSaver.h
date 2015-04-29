#pragma once

#include "../../src/classes/BDb.h"
#include "../data/DataBillingContainer.h"

class CallsSaver {
protected:
    BDb * db;
    DataBillingContainer * billingData;
public:
    CallsSaver();
    CallsSaver(BDb *db);

    void setDb(BDb *db);
    void setBillingData(DataBillingContainer * billingData);

    size_t save(const size_t save_part_count);

};

