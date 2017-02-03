#include "EventLoadStatus.h"
#include "../classes/Repository.h"
#include "../threads/ThreadLoader.h"

EventLoadStatus::EventLoadStatus() : HealthCheck("EventLoadStatus") {

}

SystemStatus EventLoadStatus::getStatus() {

    healthStatus.reset();
    Repository repository;
    if (!repository.prepare(time(nullptr))) {

        healthStatus.statusId = HealthStatus::STATUS_CRITICAL;
        healthStatus.statusMessage = "Billing not ready: "+repository.notReadyObj;
        return healthStatus;
    }
    Server *server = repository.getServer(app().conf.instance_id);
    if (server!=nullptr && server->local_event_count.size()>2 &&
        !(server->local_event_count[0]==0 && server->local_event_count[1]==0 && server->local_event_count[2]==0)) {

        int local_event_count =0;
        app().threads.forAllThreads([&](Thread* thread) {

            if (thread->id=="loader") {

                local_event_count = ((ThreadLoader*)thread)->events_count();
            }
            return true;
        });
        healthStatus.itemValue = to_string(local_event_count);

        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(server->local_event_count[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(server->local_event_count[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(server->local_event_count[2],HealthStatus::STATUS_ERROR)
        }, local_event_count);
        healthStatus.statusMessage = healthStatus.itemValue+" local events waiting";
    }
    return healthStatus;
}