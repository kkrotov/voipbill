#include "CdrWaitProcessing.h"
#include "../data/DataBillingContainer.h"
#include "../classes/Repository.h"

CdrWaitProcessing::CdrWaitProcessing() : HealthCheck("CdrWaitProcessing") {
}

SystemStatus CdrWaitProcessing::getStatus() {

    if (app().conf.cdr_proc_wait_count.size()<3) {

        healthStatus.statusMessage = "Error in system config file: key value cdr_proc_wait_count undefined";
        return healthStatus;
    }
    Repository repository;
    if (repository.prepare(time(nullptr))) {

        DataBillingContainer *billingData = repository.billingData;
        time_t cdr_count = billingData->cdrsQueueSize();
        healthStatus.itemValue = to_string(cdr_count);
        healthStatus.statusMessage = "Number of CDRs waiting to be processed: "+ to_string(cdr_count);

        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(app().conf.cdr_proc_wait_count[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(app().conf.cdr_proc_wait_count[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(app().conf.cdr_proc_wait_count[2],HealthStatus::STATUS_ERROR)
        }, cdr_count);
    }
    return healthStatus;
}