#pragma once

#include "HealthCheckController.h"
#include "../classes/BDb.h"
#include "../classes/AppBill.h"
#include "../classes/DbException.h"

class DbMainConnectStatus : public HealthCheck {

    BDb db_main;
public:
    DbMainConnectStatus() : HealthCheck("DbMainConnect") {

        db_main.setCS(app().conf.db_main);
        healthStatus.statusId = HealthStatus::STATUS_UNKNOWN;
    }
    SystemStatus getStatus() override;
};

SystemStatus DbMainConnectStatus::getStatus() {

    try {

        db_main.connect();
        db_main.query("select 1");
        healthStatus.statusId = HealthStatus::STATUS_OK;
        healthStatus.statusMessage = "";
        db_main.disconnect();
    }
    catch (DbException &e) {

        healthStatus.statusId = HealthStatus::STATUS_ERROR;
        healthStatus.statusMessage = e.what();
    }
    return healthStatus;
}

class DbCallsConnectStatus : public HealthCheck {

    BDb db_calls;
public:
    DbCallsConnectStatus() : HealthCheck("DbCallsConnect") {

        db_calls.setCS(app().conf.db_calls);
        healthStatus.statusId = HealthStatus::STATUS_UNKNOWN;
    }
    SystemStatus getStatus() override;
};

SystemStatus DbCallsConnectStatus::getStatus() {

    try {

        db_calls.connect();
        db_calls.query("select 1");
        healthStatus.statusId = HealthStatus::STATUS_OK;
        healthStatus.statusMessage = "";
        db_calls.disconnect();
    }
    catch (DbException &e) {

        healthStatus.statusId = HealthStatus::STATUS_ERROR;
        healthStatus.statusMessage = e.what();
    }
    return healthStatus;
}
