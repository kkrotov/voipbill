#pragma once

#include "HealthCheckController.h"

class TaskThreadStatus : public HealthCheck {

public:
    TaskThreadStatus () : HealthCheck("TasksThread") {

    }
    SystemStatus getStatus() override;
};
