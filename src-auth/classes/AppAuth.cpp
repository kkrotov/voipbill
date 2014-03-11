#include "../classes/AppAuth.h"
#include "../../src/threads/Thread.h"

#include "../threads/ThreadLoader.h"
#include "../threads/ThreadWeb.h"
#include "../threads/ThreadTasks.h"
#include "../../src/threads/ThreadLog.h"

#include "../../src/classes/LogWriterScreen.h"
#include "../../src/classes/LogWriterFile.h"
#include "../../src/classes/LogWriterSyslog.h"

App & app() {
    static AppAuth appVar;
    return appVar;
}

AppAuth & appAuth() {
    return (AppAuth &) app();
}

AppAuth::AppAuth() {
    init_sync_done = false;
    init_load_counters_done = false;
    init_load_data_done = false;
    init_bill_runtime_started = false;
}

void AppAuth::runApp() {

    ThreadWeb web;

    boost::thread web_thread(web);

    threads.run(new ThreadLog());
    threads.run(new ThreadLoader());
    threads.run(new ThreadTasks());

    web_thread.join();
}

void AppAuth::initLogger() {

    logger.setLogGroupingInterval(conf.log_grouping_interval);

    logger.addLogWriter(pLogWriter(new LogWriterScreen()));

    if (!conf.log_file_filename.empty())
        logger.addLogWriter(pLogWriter(new LogWriterFile(conf.log_file_filename, conf.log_file_min_level, conf.log_file_max_level)));

    if (!conf.log_file2_filename.empty())
        logger.addLogWriter(pLogWriter(new LogWriterFile(conf.log_file2_filename, conf.log_file2_min_level, conf.log_file2_max_level)));

    if (!conf.log_syslog_ident.empty())
        logger.addLogWriter(pLogWriter(new LogWriterSyslog(conf.log_syslog_ident, conf.log_syslog_min_level, conf.log_syslog_max_level)));

}