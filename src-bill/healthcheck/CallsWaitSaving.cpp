#include "CallsWaitSaving.h"
#include "../data/DataBillingContainer.h"
#include "../classes/Repository.h"

CallsWaitSaving::CallsWaitSaving() : HealthCheck("CallsWaitSaving") {
}

SystemStatus CallsWaitSaving::getStatus() {

    if (app().conf.call_save_wait_count.size()<3) {

        healthStatus.statusMessage = "Error in system config file: key value call_save_wait_count undefined";
        return healthStatus;
    }
    Repository repository;
    if (repository.prepare(time(nullptr))) {

        DataBillingContainer *billingData = repository.billingData;
        time_t call_count = billingData->callsQueueSize();
        healthStatus.itemValue = to_string(call_count);
        healthStatus.statusMessage = "Number of calls waiting to be processed: "+ to_string(call_count);

        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(app().conf.call_save_wait_count[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(app().conf.call_save_wait_count[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(app().conf.call_save_wait_count[2],HealthStatus::STATUS_ERROR)
        }, call_count);
    }
    return healthStatus;
}