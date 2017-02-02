#include "EventManagementStatus.h"
#include "../classes/Repository.h"
#include "../sync/ManagerPull.h"

EventManagementStatus::EventManagementStatus() : HealthCheck("EventManagementStatus") {

}

SystemStatus EventManagementStatus::getStatus() {

    healthStatus.reset();
    Repository repository;
    if (!repository.prepare(time(nullptr))) {

        healthStatus.statusId = HealthStatus::STATUS_CRITICAL;
        healthStatus.statusMessage = "Billing not ready";
        return healthStatus;
    }
    Server *server = repository.getServer(app().conf.instance_id);
    if (server!=nullptr && server->event_count.size()>2 &&
        !(server->event_count[0]==0 && server->event_count[1]==0 && server->event_count[2]==0)) {

        auto manager = ManagerPull::instance();
        std::vector<std::pair<time_t, HealthStatus>> eventmap = {

                std::pair<time_t, HealthStatus>(server->event_count[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(server->event_count[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(server->event_count[2],HealthStatus::STATUS_ERROR)
        };
        checkStatus (eventmap, manager->event_count);
    }
    return healthStatus;
}