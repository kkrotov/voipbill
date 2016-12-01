#include "CallSaveStatus.h"
#include "../classes/Repository.h"

CallSaveStatus::CallSaveStatus() : HealthCheck("CallSaveStatus") {
}

SystemStatus CallSaveStatus::getStatus() {

    if (app().conf.call_save_delay.size()<3) {

        healthStatus.statusMessage = "Error in system config file: key value call_save_delay undefined";
        return healthStatus;
    }
    Repository repository;
    if (repository.prepare(time(nullptr))) {

        time_t curtime;
        time(&curtime);
        DataBillingContainer *billingData = repository.billingData;
        time_t delay = curtime - billingData->getCallsStoredLastTime();
        healthStatus.statusMessage = "Delay is "+ to_string(delay)+" sec";

        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(app().conf.call_save_delay[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(app().conf.call_save_delay[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(app().conf.call_save_delay[2],HealthStatus::STATUS_ERROR)
        }, delay);
    }
    return healthStatus;
}