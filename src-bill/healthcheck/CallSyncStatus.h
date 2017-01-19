#pragma once
#include "HealthCheckController.h"

class CallSyncStatus : public HealthCheck {

public:
    CallSyncStatus();
    SystemStatus getStatus() override;
};
