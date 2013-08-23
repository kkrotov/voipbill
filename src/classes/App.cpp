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
#include "../threads/ThreadLog.h"

#include "Daemon.h"

#include "LogWriterScreen.h"
#include "LogWriterFile.h"

App::App() {
    init_sync_done = false;
    init_load_counters_done = false;
    init_load_data_done = false;
    init_bill_runtime_started = false;
}

bool App::init(int argc, char* argv[]) {

    srand(time(0));

    if (!conf.init(argc, argv))
        return false;

    return true;
}

void App::run() {

    logger.setLogGroupingInterval(conf.log_grouping_interval);
    logger.addLogWriter(pLogWriter(new LogWriterScreen()));
    logger.addLogWriter(pLogWriter(new LogWriterFile(conf.log_file, LogLevel::DEBUG, LogLevel::WARNING)));
    logger.addLogWriter(pLogWriter(new LogWriterFile(conf.err_log_file, LogLevel::ERROR, LogLevel::CRITICAL)));

    Daemoin::setPidFile();
    Daemoin::initSignalHandler();


    ThreadWeb web;

    std::thread web_thread(web);

    runThread(new ThreadLog());
    runThread(new ThreadSync());
    runThread(new ThreadLoader());
    runThread(new ThreadSaveCounters());
    runThread(new ThreadBlacklist());
    runThread(new ThreadLimitControl());
    runThread(new ThreadBillRuntime());
    runThread(new ThreadCheckStartTable());
    runThread(new ThreadCurrentCalls());
    runThread(new ThreadTasks());

    web_thread.join();
}

void App::runThread(Thread * thread) {
    thread->start();
}

void App::register_thread(Thread * thread) {
    threads_mutex.lock();
    list<Thread*>::iterator it = threads.begin();
    while (it != threads.end()) {
        Thread * tmp = *it;
        if (tmp->id == thread->id) {
            if (tmp == thread) {
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

void App::unregister_thread(Thread * thread) {
    threads_mutex.lock();
    list<Thread*>::iterator it = threads.begin();
    while (it != threads.end()) {
        if (*it == thread) {
            threads.erase(it);
            break;
        }
        ++it;
    }
    threads_mutex.unlock();
}
