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

    threads.run(new ThreadLog());
    threads.run(new ThreadSync());
    threads.run(new ThreadSyncCounters());
    threads.run(new ThreadSyncCalls());
    threads.run(new ThreadSelectCurrentCalls());
    threads.run(new ThreadSelectGlobalCounters());
    threads.run(new ThreadLoader());
    threads.run(new ThreadBlacklist());
    threads.run(new ThreadBillRuntime());
    threads.run(new ThreadLimitControl());
    threads.run(new ThreadCheckStartTable());
    threads.run(new ThreadTasks());
    threads.run(new ThreadUdpServer());

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