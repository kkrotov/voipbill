#pragma once

#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class Billing {
public:
    DataContainer *data;
    DataBillingContainer *billingData;

public:
    Billing();

    void setData(DataContainer *data);
    void setBillingData(DataBillingContainer *billingData);

    void calcCurrentCalls();
    void calc();
private:
    void updateClientCounters(Call &call, PreparedData &preparedData, ClientCounter * clientCounter);
    void updateFreeMinsCounters(Call &call, FminCounter * fminCounter);
};

