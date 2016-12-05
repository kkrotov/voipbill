#include "CallSyncStatus.h"
#include "../classes/Repository.h"

CallSyncStatus::CallSyncStatus() : HealthCheck ("CallSyncStatus") {
}

SystemStatus CallSyncStatus::getStatus() {

    if (app().conf.call_sync_delay.size()<3) {

        healthStatus.statusMessage = "Error in system config file: key value call_sync_delay undefined";
        return healthStatus;
    }
    Repository repository;
    if (repository.prepare(time(nullptr))) {

        DataBillingContainer *billingData = repository.billingData;
        time_t delay = billingData->getCallsStoredLastTime() - billingData->lastSyncCentralCallTime;
        healthStatus.itemValue = to_string(delay);
        healthStatus.statusMessage = "Delay is "+ to_string(delay)+" sec";

        std::vector<std::pair<time_t, HealthStatus>> delaymap = {

                std::pair<time_t, HealthStatus>(app().conf.call_sync_delay[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(app().conf.call_sync_delay[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(app().conf.call_sync_delay[2],HealthStatus::STATUS_ERROR)
        };
        checkStatus (delaymap, delay);
    }
    return healthStatus;
}