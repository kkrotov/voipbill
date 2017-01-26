#include "CallSaveStatus.h"
#include "../classes/Repository.h"

CallSaveStatus::CallSaveStatus() : HealthCheck("CallSaveStatus") {
}

SystemStatus CallSaveStatus::getStatus() {

    Repository repository;
    if (!repository.prepare(time(nullptr))) {

        healthStatus.statusId = HealthStatus::STATUS_CRITICAL;
        healthStatus.statusMessage = "Billing not ready";
        return healthStatus;
    }
    Server *server = repository.getServer(app().conf.instance_id);
    if (server!=nullptr && server->call_save_delay.size()>2) {

        time_t curtime;
        time(&curtime);
        DataBillingContainer *billingData = repository.billingData;
        time_t delay = curtime - billingData->getCallsStoredLastTime();
        healthStatus.itemValue = to_string(delay);
        healthStatus.statusMessage = "Delay is "+ to_string(delay)+" sec";

        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                        std::pair<time_t, HealthStatus>(server->call_save_delay[0],HealthStatus::STATUS_OK),
                        std::pair<time_t, HealthStatus>(server->call_save_delay[1],HealthStatus::STATUS_WARNING),
                        std::pair<time_t, HealthStatus>(server->call_save_delay[2],HealthStatus::STATUS_ERROR)
                }, delay);
    }
    return healthStatus;
}