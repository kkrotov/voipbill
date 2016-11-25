#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum HealthStatus  {

    STATUS_UNKNOWN, STATUS_OK, STATUS_WARNING, STATUS_ERROR, STATUS_CRITICAL
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SystemStatus {

public:
    std::string systemId;
    HealthStatus statusId;
    std::string statusMessage;

public:
    SystemStatus(std::string id) {systemId=id;statusId=STATUS_UNKNOWN;statusMessage="";};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HealthCheck {

public:
    SystemStatus healthStatus;

public:
    HealthCheck(std::string id) : healthStatus(id) {    };
    virtual SystemStatus getStatus() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HealthCheckController {

    std::mutex mutex;
    //std::map<std::string, HealthCheck * (*)()> healthCheckConstructorsMap;
    //template<class T> void registerHealthChecker(HealthCheck healthCheck);
    std::vector<std::shared_ptr<HealthCheck>> healthCheckList;

public:
    HealthCheckController();
    void add(std::shared_ptr<HealthCheck> check);
    std::vector<SystemStatus> getStatus();
};
