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
#include "LogWriterSyslog.h"

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
    logger.addLogWriter(pLogWriter(new LogWriterSyslog("voipbill", LogLevel::DEBUG, LogLevel::CRITICAL)));

    Daemoin::setPidFile();
    Daemoin::initSignalHandler();


    ThreadWeb web;

    std::thread web_thread(web);

    threads.run(new ThreadLog());
    threads.run(new ThreadSync());
    threads.run(new ThreadLoader());
    threads.run(new ThreadSaveCounters());
    threads.run(new ThreadBlacklist());
    threads.run(new ThreadLimitControl());
    threads.run(new ThreadBillRuntime());
    threads.run(new ThreadCheckStartTable());
    threads.run(new ThreadCurrentCalls());
    threads.run(new ThreadTasks());

    web_thread.join();
}
