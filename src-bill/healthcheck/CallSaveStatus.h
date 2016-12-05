#pragma once

#include "HealthCheckController.h"

class CallSaveStatus : public HealthCheck {

public:
    CallSaveStatus();
    SystemStatus getStatus() override;
};
