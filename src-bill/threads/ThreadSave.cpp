#include "ThreadSave.h"
#include "../classes/AppBill.h"

ThreadSave::ThreadSave() {
    id = idName();
    name = "Save";
    db_calls.setCS(app().conf.db_calls);
}

void ThreadSave::run() {
    unique_lock<mutex> lock(repository.billingData->saveLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    repository.billingData->save(&db_calls);
}