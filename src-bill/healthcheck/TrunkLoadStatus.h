#pragma once
#include "HealthCheckController.h"
#include "../classes/Repository.h"

class TrunkLoadStatus : public HealthCheck {

    Repository repository;
    bool ready();
public:
    TrunkLoadStatus();
    SystemStatus getStatus() override;
};
