#pragma once

#include "HealthCheckController.h"

class ThreadErrorStatus : public HealthCheck {

public:
    ThreadErrorStatus();
    SystemStatus getStatus() override;
};
