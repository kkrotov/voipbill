#include "ThreadFetchCdr.h"
#include "../classes/AppBill.h"


ThreadFetchCdr::ThreadFetchCdr() {
    id = idName();
    name = "Fetch CDR";

    db_calls.setCS(app().conf.db_calls);

    cdrLoader.setDb(&db_calls);
    cdrLoader.setBillingData(repository.billingData);
}

bool ThreadFetchCdr::ready() {
    return repository.billingData->ready();
}

void ThreadFetchCdr::run() {

    const size_t rows_per_request = 25000;

    unique_lock<mutex> lock(repository.billingData->fetchCdrLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    while (!cdrLoader.load(rows_per_request)) {

    }

}
