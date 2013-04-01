#include "App.h"
#include "../tasks/Task.h"

#include "../tasks/TaskBlacklist.h"
#include "../tasks/TaskBillRuntime.h"
#include "../tasks/TaskLimitControl.h"
#include "../tasks/TaskLoader.h"
#include "../tasks/TaskSync.h"
#include "../tasks/TaskSaveCounters.h"
#include "../tasks/TaskCheckStartTable.h"
#include "../tasks/TaskCurrentCalls.h"
#include "../tasks/TaskWeb.h"

#include "Daemon.h"

App::App()
{
    init_sync_done = false;
    init_load_counters_done = false;
    init_load_data_done = false;
    init_bill_runtime_started = false;
}

bool App::init(int argc, char* argv[]){

    srand(time(0));

    if (!conf.init(argc, argv))
        return false;

    return true;
}

void App::run()
{
    Daemoin::setPidFile();
    Daemoin::initSignalHandler();


    TaskWeb web;

    std::thread web_thread(web);

    TaskSync * task_sync = new TaskSync();
    TaskLoader * task_loader = new TaskLoader();
    TaskSaveCounters * task_savecounters = new TaskSaveCounters();
    TaskBlacklist * task_blacklist = new TaskBlacklist();
    TaskLimitControl * task_limitcontrol = new TaskLimitControl();
    TaskBillRuntime * task_billruntime = new TaskBillRuntime();
    TaskCheckStartTable * task_checkstarttable = new TaskCheckStartTable();
    TaskCurrentCalls * task_currentcalls = new TaskCurrentCalls();


    task_sync->start();
    task_loader->start();
    task_billruntime->start();
    task_limitcontrol->start();
    task_blacklist->start();
    task_savecounters->start();
    task_currentcalls->start();
    task_checkstarttable->start();

    web_thread.join();
}

void App::register_task(Task * task)
{
    tasks_mutex.lock();
    list<Task*>::iterator it = tasks.begin();
    while(it != tasks.end()){
        Task * tmp = *it;
        if (tmp->id == task->id){
            if (tmp == task)
            {
                tasks_mutex.unlock();
                return;
            }
            task->id = task->id + string_fmt("%d", rand());
            break;
        }
        ++it;
    }
    tasks.push_back(task);
    tasks_mutex.unlock();
}

void App::unregister_task(Task * task)
{
    tasks_mutex.lock();
    list<Task*>::iterator it = tasks.begin();
    while(it != tasks.end()){
        if (*it == task)
        {
            tasks.erase(it);
            break;
        }
        ++it;
    }
    tasks_mutex.unlock();
}
