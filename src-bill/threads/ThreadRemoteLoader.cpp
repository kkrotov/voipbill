#include "ThreadRemoteLoader.h"
#include "../classes/AppBill.h"

void ThreadRemoteLoader::run() {

    data->globalCounters.load(&db_main);

}

ThreadRemoteLoader::ThreadRemoteLoader() {
    id = idName();
    name = "Remote Loader";
    db_main.setCS(app().conf.db_main);

    data = DataContainer::instance();

    threadSleepSeconds = app().conf.global_counters_select_interval;
}


