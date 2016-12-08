#include "RadiusAuthServerStatus.h"
#include "../data/DataBillingContainer.h"
#include "../classes/Repository.h"
#include "../threads/ThreadRadiusAuthServer.h"

RadiusAuthServerStatus::RadiusAuthServerStatus() : HealthCheck("RadiusAuthServerStatus") {
}

SystemStatus RadiusAuthServerStatus::getStatus() {

    if (app().conf.radius_resquest_delay.size()<3)
        return healthStatus;

    Repository repository;
    if (repository.prepare(time(nullptr))) {

        time_t last_request_time;
        app().threads.forAllThreads([&](Thread* thread) {

                if (thread->id=="radius_auth_server") {

                    last_request_time = ((ThreadRadiusAuthServer*)thread)->lastRequestTime();
                }
                return true;
            });

        int last_resquest_delay = time(NULL) - last_request_time;
        healthStatus.itemValue = to_string(last_resquest_delay);

        vector<int> radius_request_delay = app().conf.radius_resquest_delay;
        checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

                std::pair<time_t, HealthStatus>(radius_request_delay[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(radius_request_delay[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(radius_request_delay[2],HealthStatus::STATUS_ERROR)
        }, last_resquest_delay);

        healthStatus.statusMessage = healthStatus.itemValue+" sec since last request";
    }
    return healthStatus;
}