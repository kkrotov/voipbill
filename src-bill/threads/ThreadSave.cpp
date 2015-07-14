#include "ThreadSave.h"
#include "../classes/AppBill.h"
#include "../classes/CallsSaver.h"

ThreadSave::ThreadSave() {
    id = idName();
    name = "Save";
    db_calls.setCS(app().conf.db_calls);
}

void ThreadSave::run() {
    const size_t save_part_count = 50000;

    unique_lock<mutex> lock(repository.billingData->saveLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    CallsSaver callsSaver;
    callsSaver.setDb(&db_calls);
    callsSaver.setBillingData(repository.billingData);

    for(;;) {
        size_t count = callsSaver.save(save_part_count);
        if (count == 0) {
            break;
        }
    }

}