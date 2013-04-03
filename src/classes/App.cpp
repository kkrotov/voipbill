#include "App.h"
#include "../threads/Thread.h"

#include "../threads/ThreadBlacklist.h"
#include "../threads/ThreadBillRuntime.h"
#include "../threads/ThreadLimitControl.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadSync.h"
#include "../threads/ThreadSaveCounters.h"
#include "../threads/ThreadCheckStartTable.h"
#include "../threads/ThreadCurrentCalls.h"
#include "../threads/ThreadWeb.h"
#include "../threads/ThreadTasks.h"

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


    ThreadWeb web;

    std::thread web_thread(web);

    ThreadSync * thread_sync = new ThreadSync();
    ThreadLoader * thread_loader = new ThreadLoader();
    ThreadSaveCounters * thread_savecounters = new ThreadSaveCounters();
    ThreadBlacklist * thread_blacklist = new ThreadBlacklist();
    ThreadLimitControl * thread_limitcontrol = new ThreadLimitControl();
    ThreadBillRuntime * thread_billruntime = new ThreadBillRuntime();
    ThreadCheckStartTable * thread_checkstarttable = new ThreadCheckStartTable();
    ThreadCurrentCalls * thread_currentcalls = new ThreadCurrentCalls();
    ThreadTasks * thread_tasks = new ThreadTasks();


    thread_sync->start();
    thread_loader->start();
    thread_billruntime->start();
    thread_limitcontrol->start();
    thread_blacklist->start();
    thread_savecounters->start();
    thread_currentcalls->start();
    thread_checkstarttable->start();
    thread_tasks->start();

    web_thread.join();
}

void App::register_thread(Thread * thread)
{
    threads_mutex.lock();
    list<Thread*>::iterator it = threads.begin();
    while(it != threads.end()){
        Thread * tmp = *it;
        if (tmp->id == thread->id){
            if (tmp == thread)
            {
                threads_mutex.unlock();
                return;
            }
            thread->id = thread->id + string_fmt("%d", rand());
            break;
        }
        ++it;
    }
    threads.push_back(thread);
    threads_mutex.unlock();
}

void App::unregister_thread(Thread * thread)
{
    threads_mutex.lock();
    list<Thread*>::iterator it = threads.begin();
    while(it != threads.end()){
        if (*it == thread)
        {
            threads.erase(it);
            break;
        }
        ++it;
    }
    threads_mutex.unlock();
}
