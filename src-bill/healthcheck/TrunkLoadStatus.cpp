#include "TrunkLoadStatus.h"
#include "../web/PageTrunks.h"

TrunkLoadStatus::TrunkLoadStatus() : HealthCheck("TrunkLoadStatus") {

}

SystemStatus TrunkLoadStatus::getStatus() {

    if (app().conf.trunk_max_load.size()<3) {

        healthStatus.statusMessage = "Error in system config file: key value trunk_max_load undefined";
        return healthStatus;
    }
    if (ready()) {

        ActiveTrunks activeTrunks(repository);
        std::string trunkName;
        int maxload = activeTrunks.maxLoad(trunkName);
        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(app().conf.trunk_max_load[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(app().conf.trunk_max_load[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(app().conf.trunk_max_load[2],HealthStatus::STATUS_ERROR)
        }, maxload);
        healthStatus.statusMessage = "Maximum trunk load is "+to_string(maxload)+"%";
        if (maxload>0)
            healthStatus.statusMessage += ", Trunk name: "+trunkName;
    }
    return healthStatus;
}

bool TrunkLoadStatus::ready() {

    if (!repository.billingData->ready())
        return false;

    return repository.prepare();
}