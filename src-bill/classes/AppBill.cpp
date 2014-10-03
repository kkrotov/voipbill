#include "../classes/AppBill.h"
#include "../../src/threads/Thread.h"
#include "../../src/classes/Daemon.h"

#include "../threads/ThreadWeb.h"
#include "../threads/ThreadLog.h"
#include "../threads/ThreadSync.h"
#include "../threads/ThreadSyncCounters.h"
#include "../threads/ThreadSyncCalls.h"
#include "../threads/ThreadSelectCurrentCalls.h"
#include "../threads/ThreadSelectGlobalCounters.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadBlacklist.h"
#include "../threads/ThreadBillRuntime.h"
#include "../threads/ThreadLimitControl.h"
#include "../threads/ThreadCheckStartTable.h"
#include "../threads/ThreadTasks.h"
#include "../threads/ThreadUdpServer.h"

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

AppBill::AppBill() {
    init_sync_done = false;
    init_load_counters_done = false;
    init_load_data_done = false;
    init_bill_runtime_started = false;
}

void AppBill::runApp() {

    Daemoin::setPidFile(conf.pid_file);

    ThreadWeb web;

    boost::thread web_thread(web);
    
    Thread* th = 0;
    std::map<std::string, Thread*> namedThreads{
        {(th = new ThreadLog(), th->id), th},
        {(th = new ThreadSync(), th->id), th},
        {(th = new ThreadSyncCounters(), th->id), th},
        {(th = new ThreadSyncCalls(), th->id), th},
        {(th = new ThreadSelectCurrentCalls(), th->id), th},
        {(th = new ThreadSelectGlobalCounters(), th->id), th},
        {(th = new ThreadLoader(), th->id), th},
        {(th = new ThreadBlacklist(), th->id), th},
        {(th = new ThreadBillRuntime(), th->id), th},
        {(th = new ThreadLimitControl(), th->id), th},
        {(th = new ThreadCheckStartTable(), th->id), th},
        {(th = new ThreadTasks(), th->id), th},
        {(th = new ThreadUdpServer(), th->id), th}
    };
    
    std::vector<std::string> standardThreads{
      "log",
      "sync",
      "sync_counters",
      "sync_calls",
      "currentcalls",
      "select_global_counters",
      "loader",
      "blacklist",
      "runtime",
      "limitcontrol",
      "checkstarttable",
      "tasks",
      "udp_server"
    };
    
    // Стандартный запуск через пул
    if (!conf.test_mode) {
        for (auto thread: standardThreads) {
            threads.run(namedThreads[thread]);
        }

        for (auto threadRec: namedThreads) {
            if (threadRec.second->getStatus() == ThreadStatus::THREAD_CREATED) {
                delete threadRec.second;
            }
        }
    } else { // Запуск потоков в рамках теста
        init_sync_done = true;
        init_bill_runtime_started = true;
        
        for (auto thread: conf.test_run_threads) {
            Thread* th = namedThreads[thread];
            if (th) {
                th->status = ThreadStatus::THREAD_RUNNING;
                th->start(true);
                th->task_thread.join();
                delete th;
                namedThreads.erase(thread);
            }
        }

        for (auto threadRec: namedThreads) {
            delete threadRec.second;
        }
        
        threads.onLastThreadExits();
    }
    
    web_thread.join();
}

void AppBill::initLogger() {

    logger.setLogGroupingInterval(conf.log_grouping_interval);

    logger.addLogWriter(pLogWriter(new LogWriterScreen()));

    if (!conf.log_file_filename.empty())
        logger.addLogWriter(pLogWriter(new LogWriterFile(conf.log_file_filename, conf.log_file_min_level, conf.log_file_max_level)));

    if (!conf.log_file2_filename.empty())
        logger.addLogWriter(pLogWriter(new LogWriterFile(conf.log_file2_filename, conf.log_file2_min_level, conf.log_file2_max_level)));

    if (!conf.log_syslog_ident.empty())
        logger.addLogWriter(pLogWriter(new LogWriterSyslog(conf.log_syslog_ident, conf.log_syslog_min_level, conf.log_syslog_max_level)));

}