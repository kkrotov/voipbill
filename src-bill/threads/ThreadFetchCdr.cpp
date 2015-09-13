#include "ThreadFetchCdr.h"
#include "../classes/AppBill.h"


ThreadFetchCdr::ThreadFetchCdr() {
    id = idName();
    name = "Fetch CDR";

    db_calls.setCS(app().conf.db_calls);
}

bool ThreadFetchCdr::ready() {
    return repository.billingData->ready();
}

void ThreadFetchCdr::run() {

    unique_lock<mutex> lock(repository.billingData->fetchCdrLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    while (!repository.billingData->cdrsLoadPart(&db_calls)) {

    }

}
