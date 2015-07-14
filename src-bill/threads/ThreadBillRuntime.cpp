#include "ThreadBillRuntime.h"

ThreadBillRuntime::ThreadBillRuntime() {
    id = idName();
    name = "Bill Runtime";

    db_calls.setCS(app().conf.db_calls);
}

void ThreadBillRuntime::run() {

    unique_lock<mutex> lock(repository.billingData->calcCallsLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    Billing billing;
    billing.calc();
}
