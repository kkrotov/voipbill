#include "HealthCheckController.h"

HealthCheckController::HealthCheckController() {

}

void HealthCheckController::add(std::shared_ptr<HealthCheck> check) {

    healthCheckList.push_back(check);
}

std::vector<SystemStatus> HealthCheckController::getStatus() {

    //lock_guard<std::mutex> lock(mutex);
    std::vector<SystemStatus> stat;
    for (auto healthCheck : healthCheckList ) {

        stat.push_back(healthCheck->getStatus());
    }
    return stat;
}
