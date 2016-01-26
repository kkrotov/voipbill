#pragma once

#include "Repository.h"

class Billing {
public:
    Repository repository;

public:
    void setData(DataContainer *data);
    void setBillingData(DataBillingContainer *billingData);

    void calcCurrentCalls();
    void calc(bool realtimePurpose = false);
};

