#include "CdrWaitProcessing.h"
#include "../data/DataBillingContainer.h"
#include "../classes/Repository.h"

CdrWaitProcessing::CdrWaitProcessing() : HealthCheck("CdrWaitProcessing") {
}

SystemStatus CdrWaitProcessing::getStatus() {

    healthStatus.reset();
    if (!app().threads.isRegistered("cdr_parser"))
        return healthStatus;

    Repository repository;
    if (!repository.prepare(time(nullptr))) {

        healthStatus.statusId = HealthStatus::STATUS_CRITICAL;
        healthStatus.statusMessage = "Billing not ready: "+repository.notReadyObj;
        return healthStatus;
    }
    Server *server = repository.getServer(app().conf.instance_id);
    if (server!=nullptr && server->cdr_proc_wait_count.size()>2 &&
        !(server->cdr_proc_wait_count[0]==0 && server->cdr_proc_wait_count[1]==0 && server->cdr_proc_wait_count[2]==0)) {

        DataBillingContainer *billingData = repository.billingData;
        time_t cdr_count = billingData->cdrsQueueSize();
        healthStatus.itemValue = to_string(cdr_count);
        healthStatus.statusMessage = "Number of CDRs waiting to be processed: "+ to_string(cdr_count);

        std::vector<std::pair<time_t, HealthStatus>> delaymap = {

                std::pair<time_t, HealthStatus>(server->cdr_proc_wait_count[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(server->cdr_proc_wait_count[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(server->cdr_proc_wait_count[2],HealthStatus::STATUS_ERROR)
        };
        checkStatus (delaymap, cdr_count);
    }
    return healthStatus;
}