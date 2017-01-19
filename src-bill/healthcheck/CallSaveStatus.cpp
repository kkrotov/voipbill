#include "CallSaveStatus.h"
#include "../classes/Repository.h"

CallSaveStatus::CallSaveStatus() : HealthCheck("CallSaveStatus") {
}

SystemStatus CallSaveStatus::getStatus() {

    healthStatus.reset();
    if (!app().threads.isRegistered("runtime"))
        return healthStatus;

    Repository repository;
    if (repository.prepare(time(nullptr))) {

        Server *server = repository.getServer(app().conf.instance_id);
        if (server!=nullptr && server->call_save_delay.size()>2 &&
            !(server->call_save_delay[0]==0 && server->call_save_delay[1]==0 && server->call_save_delay[2]==0)) {

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
    }
    return healthStatus;
}