#pragma once

#include "HealthCheckController.h"

class CallsWaitSaving : public HealthCheck {

public:
    CallsWaitSaving();
    SystemStatus getStatus() override;
};
