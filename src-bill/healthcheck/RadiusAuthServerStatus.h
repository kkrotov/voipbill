#pragma once

#include "HealthCheckController.h"

class RadiusAuthServerStatus : public HealthCheck {

public:
    RadiusAuthServerStatus();
    SystemStatus getStatus() override;
};
