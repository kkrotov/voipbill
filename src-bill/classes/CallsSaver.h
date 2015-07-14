#pragma once

#include "BDb.h"
#include "Repository.h"

class CallsSaver {
protected:
    BDb * db;
    Repository repository;
public:
    CallsSaver();
    CallsSaver(BDb *db);

    void setDb(BDb *db);
    void setBillingData(DataBillingContainer * billingData);

    size_t save(const size_t save_part_count);

};

