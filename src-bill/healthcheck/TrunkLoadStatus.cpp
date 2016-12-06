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
        vector<pair<int,string>> trunkLoad = activeTrunks.getLoad();
        std::sort(trunkLoad.begin(), trunkLoad.end(), [](pair<int,string> a, pair<int,string> b) {

            return b.first>a.first; // asc
        });
        healthStatus.statusMessage = "";
        healthStatus.statusId = HealthStatus::STATUS_UNKNOWN;
        for (auto load : trunkLoad) {

            checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                    std::pair<time_t, HealthStatus>(app().conf.trunk_max_load[0],HealthStatus::STATUS_OK),
                    std::pair<time_t, HealthStatus>(app().conf.trunk_max_load[1],HealthStatus::STATUS_WARNING),
                    std::pair<time_t, HealthStatus>(app().conf.trunk_max_load[2],HealthStatus::STATUS_ERROR)
            }, load.first);
            if (healthStatus.statusId == HealthStatus::STATUS_OK)
                continue;

            healthStatus.itemValue = to_string(load.first);
            if (healthStatus.statusMessage.size()>0)
                healthStatus.statusMessage += "; ";

            healthStatus.statusMessage += "Trunk name: "+load.second+", Trunk load: "+to_string(load.first)+"%";
        }
    }
    return healthStatus;
}

bool TrunkLoadStatus::ready() {

    if (!repository.billingData->ready())
        return false;

    return repository.prepare();
}