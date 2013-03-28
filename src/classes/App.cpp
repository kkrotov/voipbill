#include "App.h"
#include "../tasks/Task.h"

App::App()
{
    init_sync_done = false;
    init_load_counters_done = false;
    init_load_data_done = false;
    init_bill_runtime_started = false;
}

bool App::init(int argc, char* argv[]){
    if (!conf.init(argc, argv))
        return false;
    return true;
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
