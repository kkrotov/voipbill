#include "ThreadFetchCdr.h"
#include "../classes/AppBill.h"


ThreadFetchCdr::ThreadFetchCdr() {
    id = idName();
    name = "Fetch CDR";

    db_calls.setCS(app().conf.db_calls);
    billingData = DataBillingContainer::instance();

    cdrLoader.setDb(&db_calls);
    cdrLoader.setBillingData(billingData);
}

bool ThreadFetchCdr::ready() {
    return billingData->ready();
}

void ThreadFetchCdr::run() {

    const size_t rows_per_request = 25000;

    unique_lock<mutex> lock(billingData->fetchCdrLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    while (!cdrLoader.load(rows_per_request)) {
        boost::this_thread::interruption_point();
    }

}

void ThreadFetchCdr::htmlfull(stringstream &html) {
    this->html(html);
    DataBillingContainer::instance()->html(html);
}
