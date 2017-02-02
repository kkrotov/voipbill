#include "EventManagementStatus.h"
#include "../classes/Repository.h"

EventManagementStatus::EventManagementStatus() : HealthCheck("EventManagementStatus") {

    db_main.setCS(app().conf.db_main);
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

        int eventnum = event_count();
        std::vector<std::pair<time_t, HealthStatus>> eventmap = {

                std::pair<time_t, HealthStatus>(server->event_count[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(server->event_count[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(server->event_count[2],HealthStatus::STATUS_ERROR)
        };
        checkStatus (eventmap, eventnum);
    }
    return healthStatus;
}

int EventManagementStatus::event_count() {

    string select_events_query = "select count(*) from event.queue where server_id in " + app().conf.get_sql_regions_list();
    BDbResult res = db_main.query(select_events_query);
    return (res.next())? res.get_i(0):0;
}