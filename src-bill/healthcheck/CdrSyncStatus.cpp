#include "CdrSyncStatus.h"
#include "../classes/Repository.h"

CdrSyncStatus::CdrSyncStatus() : HealthCheck("CdrSyncStatus") {
}

SystemStatus CdrSyncStatus::getStatus() {

    if (app().conf.cdr_sync_delay.size()<3) {

        healthStatus.statusMessage = "Error in system config file: key value cdr_sync_delay undefined";
        return healthStatus;
    }
    Repository repository;
    if (repository.prepare(time(nullptr))) {

        DataBillingContainer *billingData = repository.billingData;
        time_t delay = billingData->getCdrsLastTime() - billingData->lastSyncCentralCdrTime;
        healthStatus.statusMessage = "Delay is "+ to_string(delay)+" sec";

        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(app().conf.cdr_sync_delay[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(app().conf.cdr_sync_delay[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(app().conf.cdr_sync_delay[2],HealthStatus::STATUS_ERROR)
        }, delay);
    }
    return healthStatus;
}