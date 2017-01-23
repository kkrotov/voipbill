#pragma once

#include "HealthCheckController.h"

class CdrSyncStatus  : public HealthCheck {

public:
    CdrSyncStatus();
    SystemStatus getStatus() override;
};
