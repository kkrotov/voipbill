#include "../classes/AppBill.h"
#include "../../src/classes/Daemon.h"

#include "../threads/ThreadWeb.h"
#include "../threads/ThreadLog.h"
#include "../threads/ThreadSync.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadRemoteLoader.h"
#include "../threads/ThreadCurrentCalls.h"
#include "../threads/ThreadFetchCdr.h"
#include "../threads/ThreadBillRuntime.h"
#include "../threads/ThreadSaveCalls.h"
#include "../threads/ThreadSyncCalls.h"
#include "../threads/ThreadSyncCounters.h"
#include "../threads/ThreadSyncLocks.h"
#include "../threads/ThreadClientLock.h"
#include "../threads/ThreadBlacklistFetch.h"
#include "../threads/ThreadBlacklistCalc.h"
#include "../threads/ThreadLimitControl.h"
#include "../threads/ThreadUpdateActiveClients.h"
#include "../threads/ThreadCheckStartTable.h"
#include "../threads/ThreadTasks.h"
#include "../../src/classes/LogWriterScreen.h"
#include "../../src/classes/LogWriterFile.h"
#include "../../src/classes/LogWriterSyslog.h"

AppBill & app() {
    static AppBill appVar;
    return appVar;
}

bool AppBill::init(int argc, char* argv[]) {

    if (!conf.init(argc, argv))
        return false;

    return App::init(argc, argv);
}

void AppBill::runApp() {

    Daemon::setPidFile(conf.pid_file);
    
    registerAllThreads();

    ThreadWeb web;

    boost::thread web_thread(boost::ref(web));
    
    if (conf.test_mode) {
        runAppInTestMode();
    } else {
        runAppInSingleMode();
    }
    
    threads.joinAll();

    web.stop();
    web_thread.join();
    
    if (conf.test_mode) {
        Log::flush();
    }
}

void AppBill::initLogger() {

    logger.setLogGroupingInterval(conf.log_grouping_interval);

    logger.addLogWriter(pLogWriter(new LogWriterScreen()));

    if (!conf.log_file_filename.empty())
        logger.addLogWriter(pLogWriter(new LogWriterFile(conf.log_file_filename, conf.log_file_min_level, conf.log_file_max_level)));

    if (!conf.log_syslog_ident.empty())
        logger.addLogWriter(pLogWriter(new LogWriterSyslog(conf.log_syslog_ident, conf.log_syslog_min_level, conf.log_syslog_max_level)));

}

template<class T>
void AppBill::registerThread() {
    threadConstructorsMap[T::idName()] = []() -> Thread * { return new T(); };
}

Thread * AppBill::newThreadObject(std::string id) {
    auto fn = threadConstructorsMap[id];
    if (fn) {
        return fn();
    } else {
        return 0;
    }
}

void AppBill::registerAllThreads() {

    registerThread<ThreadLog>();
    registerThread<ThreadSync>();
    registerThread<ThreadLoader>();
    registerThread<ThreadRemoteLoader>();
    registerThread<ThreadCurrentCalls>();
    registerThread<ThreadFetchCdr>();
    registerThread<ThreadBillRuntime>();
    registerThread<ThreadSaveCalls>();
    registerThread<ThreadSyncCalls>();
    registerThread<ThreadSyncCounters>();
    registerThread<ThreadSyncLocks>();
    registerThread<ThreadClientLock>();
    registerThread<ThreadBlacklistFetch>();
    registerThread<ThreadBlacklistCalc>();
    registerThread<ThreadLimitControl>();
    registerThread<ThreadUpdateActiveClients>();
    registerThread<ThreadCheckStartTable>();
    registerThread<ThreadTasks>();
}

void AppBill::runAppInSingleMode()
{
    std::vector<std::string> standardThreads {
            "log",
            "sync",
            "loader",
            "remote_loader",
            "current_calls",
            "fetch_cdr",
            "runtime",
            "save_calls",
            "sync_calls",
            "sync_counters",
            "sync_locks",
            "client_lock",
            "blacklist_fetch",
            "blacklist_calc",
            "limitcontrol",
            "update_active_clients",
            "checkstarttable",
            "tasks",
    };

    for (auto thread: standardThreads) {
        threads.run(newThreadObject(thread));
    }
}

void AppBill::runAppInTestMode()
{
    for (auto threadName: conf.test_threads) {
        Thread* thread = newThreadObject(threadName);
        if (thread) {
            thread->status = ThreadStatus::THREAD_RUNNING;
            int runsCount = conf.test_threads_runs_count[threadName];
            bool skipPrepare = conf.test_threads_skip_prepare[threadName];
            thread->start(runsCount, skipPrepare);
            thread->task_thread.join();
            delete thread;
        } else {
            Log::error("UNKNOWN THREAD " + threadName);
        }
    }
   
    setStatus(AppStatus::APP_STOPPED);
}