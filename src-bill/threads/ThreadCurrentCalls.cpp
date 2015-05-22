#include "ThreadCurrentCalls.h"
#include "../classes/Billing.h"


void ThreadCurrentCalls::run() {
    currentCalls->currentCdr.load(&db_calls);

    Billing billing;
    billing.calcCurrentCalls();


    auto currentCdr = currentCalls->currentCdr.get();
    int currentCallPerSecond = 0;
    size_t count = currentCdr->size();
    if (count > 0) {
        for (size_t i = count; i > 0; i--) {
            Cdr *cdr = currentCdr->get(i - 1);
            if (cdr->call_id > currentCalls->lastCallId) {
                currentCallPerSecond += 1;
            }
        }
        currentCalls->lastCallId = currentCdr->get(count - 1)->call_id;
    }

    if (currentCalls->callPerSecond == -1) {
        currentCalls->callPerSecond = 0;
    } else {
        currentCalls->callPerSecond = currentCallPerSecond;
    }

    if (currentCalls->callPerSecond > currentCalls->maxCallPerSecond) {
        currentCalls->maxCallPerSecond = currentCalls->callPerSecond;
    }
}

ThreadCurrentCalls::ThreadCurrentCalls() {
    id = idName();
    name = "Current Calls";
    db_calls.setCS(app().conf.db_calls);
    currentCalls = DataCurrentCallsContainer::instance();
}


