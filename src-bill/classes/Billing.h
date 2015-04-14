#pragma once

#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class Billing {
public:
    DataContainer *data;
    DataBillingContainer *billingData;
    DataBillingContainer *dstBillingData;

public:
    Billing();

    void setData(DataContainer *data);
    void setBillingData(DataBillingContainer *billingData);
    void setDstBillingData(DataBillingContainer *billingData);

    void calcCurrentCdrs();
    void calcCdrs();

protected:
    void calc(Call &call);
    void initCall(Call &call, Cdr * cdr);
};

