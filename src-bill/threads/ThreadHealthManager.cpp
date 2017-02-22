#include "ThreadHealthManager.h"

ThreadHealthManager::ThreadHealthManager() {

    id = idName();
    name = "Trunk Load Status Manager";
    trunk_critical_load_delay = app().conf.trunk_critical_load_delay;
    threadSleepSeconds = 1;
}

void ThreadHealthManager::run() {

    if (!ready())
        return;

    SystemStatus trunkLoadStatus = app().healthCheckController.getStatus("TrunkLoadStatus");
    if (trunkLoadStatus.statusId==HealthStatus::STATUS_OK || trunkLoadStatus.statusId==HealthStatus::STATUS_WARNING) {

        trunk_error_time[trunkLoadStatus.itemName] = 0;
        return;
    }
    time_t new_critical_err_time = time(NULL);
    time_t last_critical_err_time = trunk_error_time[trunkLoadStatus.itemName];
    if (last_critical_err_time==0) {

        trunk_error_time[trunkLoadStatus.itemName] = new_critical_err_time;
        return;
    }
    if (new_critical_err_time-last_critical_err_time > trunk_critical_load_delay) {

        pLogMessage logTrunk(new LogMessage());
        logTrunk->level = LogLevel::ERROR;
        logTrunk->type = "trunk";
        logTrunk->params["_trunk_name"] = trunkLoadStatus.itemName;
        logTrunk->params["_trunk_load"] = trunkLoadStatus.itemValue;
        logTrunk->params["status"] = trunkLoadStatus.getStatusString();
        logTrunk->params["time_delay"] = trunk_critical_load_delay;
        logTrunk->log_time = new_critical_err_time;
        logTrunk->message = "Trunk "+trunkLoadStatus.itemName+" overload for more than "+std::to_string(trunk_critical_load_delay)+" sec";
        app().logger.logMessage(logTrunk);

        trunk_error_time[trunkLoadStatus.itemName] = new_critical_err_time;
    }
}
