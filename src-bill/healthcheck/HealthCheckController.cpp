#include "HealthCheckController.h"

SystemStatus HealthCheck::checkStatus(std::vector<std::pair<time_t, HealthStatus>> &delaymap, time_t delay) {

    if (delaymap.size() > 0) {

        std::lock_guard<std::mutex> lock(mutex);
        int i;
        for (i=0; i<delaymap.size(); i++) {

            if (delaymap[i].first >= delay) {

                break;
            }
        }
        healthStatus.itemValue = std::to_string(delay);
        healthStatus.prevValue = healthStatus.nextValue = "";
        if (i>0) {

            healthStatus.prevValue = std::to_string(delaymap[i-1].first);
        }
        if (i>=delaymap.size()) {

            healthStatus.statusId = HealthStatus::STATUS_CRITICAL;
        }
        else {

            healthStatus.statusId = delaymap[i].second;
            healthStatus.nextValue = std::to_string(delaymap[i].first);
        }
    }
    return healthStatus;
}

void HealthCheckController::add(std::shared_ptr<HealthCheck> check) {

    healthCheckList.push_back(check);
}

bool HealthCheckController::getStatus(std::vector<SystemStatus>  &stat) {

    std::lock_guard<std::mutex> lock(mutex);
    stat.clear();
    for (auto healthCheck : healthCheckList ) {

        stat.push_back(healthCheck->getStatus());
    }
    return !stat.empty();
}

SystemStatus HealthCheckController::getStatus(std::string id) {

    for (auto healthCheck : healthCheckList ) {

        if (healthCheck->getSystemId() == id)
            return healthCheck->getStatus();
    }
    return SystemStatus(id);
}