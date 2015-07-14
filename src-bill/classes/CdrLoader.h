#pragma once

#include "Repository.h"

class CdrLoader {
private:
    BDb * db_calls;
    Repository repository;
public:

    void setDb(BDb * db_calls);

    void setBillingData(DataBillingContainer * billingData);

    bool load(const size_t rows_per_request);
};