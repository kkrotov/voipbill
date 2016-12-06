#include "TrunkLoadStatus.h"
#include "../web/PageTrunks.h"

TrunkLoadStatus::TrunkLoadStatus() : HealthCheck("TrunkLoadStatus") {

}

SystemStatus TrunkLoadStatus::getStatus() {

    if (!ready())
        return healthStatus;

    int trunk_max_load_warn = (app().conf.trunk_max_load.size()>0)? app().conf.trunk_max_load[1]:95;
    int trunk_max_load_err = (app().conf.trunk_max_load.size()>1)? app().conf.trunk_max_load[2]:99;
    ActiveTrunks activeTrunks(repository);
    vector<pair<int,string>> trunkLoad = activeTrunks.getLoad();
    std::sort(trunkLoad.begin(), trunkLoad.end(), [](pair<int,string> a, pair<int,string> b) {

        return b.first>a.first; // asc
    });
    healthStatus.statusMessage = "";
    healthStatus.statusId = HealthStatus::STATUS_OK;
    for (auto load : trunkLoad) {

        string trunkName = load.second;
        Trunk *trunk = repository.getTrunkByName(trunkName.c_str());
        if (trunk== nullptr)
            continue;

        int trunk_max_load_ok= trunk->load_warning;
        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(trunk_max_load_ok,HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(trunk_max_load_warn,HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(trunk_max_load_err,HealthStatus::STATUS_ERROR)
        }, load.first);
        if (healthStatus.statusId == HealthStatus::STATUS_OK)
            continue;

        healthStatus.itemValue = to_string(load.first);
        if (healthStatus.statusMessage.size()>0)
            healthStatus.statusMessage += "; ";

        healthStatus.statusMessage += "Trunk name: "+trunkName+", Trunk load: "+to_string(load.first)+"%";
    }
    return healthStatus;
}

bool TrunkLoadStatus::ready() {

    if (!repository.billingData->ready())
        return false;

    return repository.prepare();
}