#pragma once

#include "HealthCheckController.h"

class CdrWaitProcessing : public HealthCheck {

public:
    CdrWaitProcessing();
    SystemStatus getStatus() override;
};
