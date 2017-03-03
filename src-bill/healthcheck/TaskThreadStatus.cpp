#include "TaskThreadStatus.h"
#include "../classes/AppBill.h"
#include "../threads/ThreadTasks.h"

SystemStatus TaskThreadStatus::getStatus() {

    if (!app().threads.isRegistered("tasks"))
        return healthStatus;

    ThreadTasks *threadTasks = nullptr;
    app().threads.forAllThreads([&](Thread* thread) {

        if (thread->id.compare("tasks")==0) {

            threadTasks = (ThreadTasks *)thread;
            return false;
        }
        return true;
    });
    shared_ptr<Task> currentTask = threadTasks->getCurrentTask();
    if (currentTask!= nullptr) {

        healthStatus.itemId = "CurrentTask";
        healthStatus.statusMessage = currentTask->getName(); ///currentTask->getStatus();
        healthStatus.statusId = HealthStatus::STATUS_WARNING;
    }
    else {

        healthStatus.itemId = "";
        healthStatus.statusMessage = "";
        healthStatus.statusId = HealthStatus::STATUS_OK;
    }
    return healthStatus;
}