#pragma once

#include "HealthCheckController.h"

class DbConnectStatus : public HealthCheck {

public:
    DbConnectStatus() : HealthCheck("DbConnect") {

        healthStatus.statusId = HealthStatus::STATUS_OK;
    }
    SystemStatus getStatus() override;
};
