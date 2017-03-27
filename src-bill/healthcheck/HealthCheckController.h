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
    std::string itemId, itemName;
    std::string itemValue, prevValue, nextValue;
    HealthStatus statusId;
    std::string statusMessage;

public:
    SystemStatus(std::string id) {itemId=id;reset();};
    void reset() {statusId=STATUS_UNKNOWN;itemValue=prevValue=nextValue=statusMessage="";};
    std::string getStatusString() {
        if (statusId==HealthStatus::STATUS_OK)
            return "STATUS_OK";
        if (statusId==HealthStatus::STATUS_WARNING)
            return "STATUS_WARNING";
        if (statusId==HealthStatus::STATUS_ERROR)
            return "STATUS_ERROR";
        if (statusId==HealthStatus::STATUS_CRITICAL)
            return "STATUS_CRITICAL";
        return "STATUS_UNKNOWN";
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HealthCheck {

public:
    SystemStatus healthStatus;

public:
    HealthCheck(std::string id) : healthStatus(id) {    };
    SystemStatus checkStatus(std::vector<std::pair<time_t, HealthStatus>> delaymap, time_t delay);
    std::string getSystemId() { return healthStatus.itemId; };
    virtual SystemStatus getStatus() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HealthCheckController {

    std::mutex mutex;
    //std::map<std::string, HealthCheck * (*)()> healthCheckConstructorsMap;
    //template<class T> void registerHealthChecker(HealthCheck healthCheck);
    std::vector<std::shared_ptr<HealthCheck>> healthCheckList;

public:
    HealthCheckController() { };
    void add(std::shared_ptr<HealthCheck> check);
    bool getStatus(std::vector<SystemStatus> &stat);
    SystemStatus getStatus(std::string id);
};
