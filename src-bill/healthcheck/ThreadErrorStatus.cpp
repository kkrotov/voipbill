#include "ThreadErrorStatus.h"
#include "../classes/Thread.h"
#include "../classes/AppBill.h"

ThreadErrorStatus::ThreadErrorStatus() : HealthCheck("ThreadErrorStatus") {

}

SystemStatus ThreadErrorStatus::getStatus() {

    if (app().conf.thread_error_count.size()<3) {

        healthStatus.statusMessage = "Error in system config file: key value thread_error_count undefined";
        return healthStatus;
    }
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
    checkStatus (std::vector<std::pair<time_t, HealthStatus>> {

            std::pair<time_t, HealthStatus>(app().conf.thread_error_count[0],HealthStatus::STATUS_OK),
            std::pair<time_t, HealthStatus>(app().conf.thread_error_count[1],HealthStatus::STATUS_WARNING),
            std::pair<time_t, HealthStatus>(app().conf.thread_error_count[2],HealthStatus::STATUS_ERROR)
    }, errorsCount);
    if (errorsCount>0) {

        healthStatus.statusMessage = "Thread id: "+threadId+", error count: "+to_string(errorsCount)+", Error message: "+errorMsg;
    }
    return healthStatus;
}
