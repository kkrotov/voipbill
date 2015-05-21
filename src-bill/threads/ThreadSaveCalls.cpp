#include "ThreadSaveCalls.h"
#include "../classes/AppBill.h"
#include "../classes/CallsSaver.h"

ThreadSaveCalls::ThreadSaveCalls() {
    id = idName();
    name = "Save calls";
    db_calls.setCS(app().conf.db_calls);
    billingData = DataBillingContainer::instance();
}

void ThreadSaveCalls::run() {
    const size_t save_part_count = 50000;

    unique_lock<mutex> lock(billingData->saveCallsLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    CallsSaver callsSaver;
    callsSaver.setDb(&db_calls);
    callsSaver.setBillingData(billingData);

    for(;;) {
        size_t count = callsSaver.save(save_part_count);
        if (count == 0) {
            break;
        }
    }

}

void ThreadSaveCalls::htmlfull(stringstream &html) {
    this->html(html);
    DataBillingContainer::instance()->html(html);
}
