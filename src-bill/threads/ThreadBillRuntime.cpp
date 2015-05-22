#include "ThreadBillRuntime.h"

ThreadBillRuntime::ThreadBillRuntime() {
    id = idName();
    name = "Bill Runtime";

    db_calls.setCS(app().conf.db_calls);
    billingData = DataBillingContainer::instance();
}

void ThreadBillRuntime::run() {

    unique_lock<mutex> lock(billingData->calcCallsLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    Billing billing;
    billing.calc();
}
