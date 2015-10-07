#include "ThreadCurrentCalls.h"
#include "../classes/Billing.h"


void ThreadCurrentCalls::run() {
    repository.currentCalls->currentCdr.load(&db_calls);

    Billing billing;
    billing.calcCurrentCalls();


    auto currentCdr = repository.currentCalls->currentCdr.get();
    int currentCallPerSecond = 0;
    size_t count = currentCdr->size();
    if (count > 0) {
        for (size_t i = count; i > 0; i--) {
            Cdr *cdr = currentCdr->get(i - 1);
            if (cdr->call_id > repository.currentCalls->lastCallId) {
                currentCallPerSecond += 1;
            }
        }
        repository.currentCalls->lastCallId = currentCdr->get(count - 1)->call_id;
    }

    if (repository.currentCalls->callPerSecond == -1) {
        repository.currentCalls->callPerSecond = 0;
    } else {
        repository.currentCalls->callPerSecond = currentCallPerSecond;
    }

    if (repository.currentCalls->callPerSecond > repository.currentCalls->maxCallPerSecond) {
        repository.currentCalls->maxCallPerSecond = repository.currentCalls->callPerSecond;
    }
}

ThreadCurrentCalls::ThreadCurrentCalls() {
    id = idName();
    name = "Current Calls";
    db_calls.setCS(app().conf.db_calls);
}


