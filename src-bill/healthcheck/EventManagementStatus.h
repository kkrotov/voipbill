#pragma once
#include "HealthCheckController.h"
#include "../classes/BDb.h"

class EventManagementStatus : public HealthCheck {

    BDb db_main;
    int event_count();
public:
    EventManagementStatus();
    SystemStatus getStatus() override;
};
