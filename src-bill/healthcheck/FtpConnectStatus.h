#pragma once

#include "HealthCheckController.h"

class FtpConnectStatus : public HealthCheck {

public:
    FtpConnectStatus() : HealthCheck("FtpConnect") {

        healthStatus.statusId = HealthStatus::STATUS_OK;
    }
    SystemStatus getStatus() override;
};
