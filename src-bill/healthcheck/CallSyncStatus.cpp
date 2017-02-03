#include "CallSyncStatus.h"
#include "../classes/Repository.h"

CallSyncStatus::CallSyncStatus() : HealthCheck ("CallSyncStatus") {
}

SystemStatus CallSyncStatus::getStatus() {

    healthStatus.reset();
    if (!app().threads.isRegistered("sync_calls"))
        return healthStatus;

    Repository repository;
    if (!repository.prepare(time(nullptr))) {

        healthStatus.statusId = HealthStatus::STATUS_CRITICAL;
        healthStatus.statusMessage = "Billing not ready: "+repository.notReadyObj;
        return healthStatus;
    }
    Server *server = repository.getServer(app().conf.instance_id);
    if (server!=nullptr && server->cdr_sync_delay.size()>2 &&
        !(server->cdr_sync_delay[0]==0 && server->cdr_sync_delay[1]==0 && server->cdr_sync_delay[2]==0)) {

        DataBillingContainer *billingData = repository.billingData;
        time_t delay = billingData->getCdrsLastTime() - billingData->lastSyncCentralCdrTime;
        healthStatus.itemValue = to_string(delay);
        healthStatus.statusMessage = "Delay is "+ to_string(delay)+" sec";

        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(server->cdr_sync_delay[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(server->cdr_sync_delay[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(server->cdr_sync_delay[2],HealthStatus::STATUS_ERROR)
        }, delay);
    }
    return healthStatus;
}