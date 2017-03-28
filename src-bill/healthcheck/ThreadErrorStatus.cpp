#include "ThreadErrorStatus.h"
#include "../classes/Thread.h"
#include "../classes/AppBill.h"
#include "../models/Server.h"
#include "../classes/Repository.h"

ThreadErrorStatus::ThreadErrorStatus() : HealthCheck("ThreadErrorStatus") {

}

SystemStatus ThreadErrorStatus::getStatus() {

    Repository repository;
    if (!repository.prepare(time(nullptr))) {

        healthStatus.statusId = HealthStatus::STATUS_CRITICAL;
        healthStatus.statusMessage = "Billing not ready: "+repository.notReadyObj;
        return healthStatus;
    }
    healthStatus.reset();
    Server *server = repository.getServer(app().conf.instance_id);
    if (server != nullptr && server->thread_error_count.size() > 2 &&
        !(server->thread_error_count[0]==0 && server->thread_error_count[1]==0 && server->thread_error_count[2]==0)) {

        int errorsCount=0;
        std::string threadId;
        std::string errorMsg;
        app().threads.forAllThreads([&](Thread* thread) {

            std::string msg;
            int cnt = thread->errorCount(msg);
            if (errorsCount < cnt) {

                errorsCount = cnt;
                errorMsg = msg;
                threadId = thread->id;
            }
            return true;
        });
        healthStatus.itemValue = to_string(errorsCount);
        std::vector<std::pair<time_t, HealthStatus>> eventmap = {

                std::pair<time_t, HealthStatus>(server->thread_error_count[0],HealthStatus::STATUS_OK),
                std::pair<time_t, HealthStatus>(server->thread_error_count[1],HealthStatus::STATUS_WARNING),
                std::pair<time_t, HealthStatus>(server->thread_error_count[2],HealthStatus::STATUS_ERROR)
        };
        checkStatus (eventmap, errorsCount);
        if (errorsCount>0) {

            healthStatus.statusMessage = "Thread id: "+threadId+", error count: "+to_string(errorsCount)+", Error message: "+errorMsg;
        }
    }
    return healthStatus;
}
