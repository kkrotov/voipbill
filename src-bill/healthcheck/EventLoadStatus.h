#pragma once

#include "HealthCheckController.h"

class EventLoadStatus : public HealthCheck {

public:
    EventLoadStatus();
    SystemStatus getStatus() override;
};
