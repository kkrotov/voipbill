#include "CallsWaitSaving.h"
#include "../data/DataBillingContainer.h"
#include "../classes/Repository.h"

CallsWaitSaving::CallsWaitSaving() : HealthCheck("CallsWaitSaving") {
}

SystemStatus CallsWaitSaving::getStatus() {

    if (!app().threads.isRegistered("current_calls"))
        return healthStatus;

    Repository repository;
    if (repository.prepare(time(nullptr))) {

        Server *server = repository.getServer(app().conf.instance_id);
        if (server!=nullptr && server->call_save_wait_count.size()>2 &&
            !(server->call_save_wait_count[0]==0 && server->call_save_wait_count[1]==0 && server->call_save_wait_count[2]==0)) {

            DataBillingContainer *billingData = repository.billingData;
            time_t call_count = billingData->callsQueueSize();
            healthStatus.itemValue = to_string(call_count);
            healthStatus.statusMessage = "Number of calls waiting to be processed: "+ to_string(call_count);

            checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                            std::pair<time_t, HealthStatus>(server->call_save_wait_count[0],HealthStatus::STATUS_OK),
                            std::pair<time_t, HealthStatus>(server->call_save_wait_count[1],HealthStatus::STATUS_WARNING),
                            std::pair<time_t, HealthStatus>(server->call_save_wait_count[2],HealthStatus::STATUS_ERROR)
                    }, call_count);
        }
    }
    return healthStatus;
}