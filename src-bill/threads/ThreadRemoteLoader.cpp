#include "ThreadRemoteLoader.h"
#include "../classes/AppBill.h"

void ThreadRemoteLoader::run() {

    repository.data->globalCounters.load(&db_main);

}

ThreadRemoteLoader::ThreadRemoteLoader() {
    id = idName();
    name = "Remote Loader";
    db_main.setCS(app().conf.db_main);

    threadSleepSeconds = app().conf.global_counters_select_interval;
}


