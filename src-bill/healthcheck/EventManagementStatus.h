#pragma once
#include "HealthCheckController.h"
#include "../classes/BDb.h"

class EventManagementStatus : public HealthCheck {

public:
    EventManagementStatus();
    SystemStatus getStatus() override;
};
