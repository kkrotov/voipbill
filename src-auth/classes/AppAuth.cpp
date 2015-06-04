#include "../classes/AppAuth.h"
#include "../../src/threads/Thread.h"
#include "../../src/classes/Daemon.h"

#include "../threads/ThreadLoader.h"
#include "../threads/ThreadWeb.h"
#include "../threads/ThreadLog.h"
#include "../threads/ThreadSync.h"
#include "../threads/ThreadUdpServer.h"

#include "../../src/classes/LogWriterScreen.h"
#include "../../src/classes/LogWriterFile.h"
#include "../../src/classes/LogWriterSyslog.h"
#include "ConfigVersionData.h"

AppAuth & app() {
    static AppAuth appVar;
    return appVar;
}

bool AppAuth::init(int argc, char* argv[]) {

    if (!conf.init(argc, argv))
        return false;

    return App::init(argc, argv);
}

AppAuth::AppAuth() {
    init_sync_done = false;
    init_load_counters_done = false;
    init_load_data_done = false;
    init_bill_runtime_started = false;
}

void AppAuth::runApp() {

    Daemon::setPidFile(conf.pid_file);

    ThreadWeb web;

    boost::thread web_thread(boost::ref(web));

    threads.run(new ThreadLog());
    threads.run(new ThreadSync());
    threads.run(new ThreadLoader());
    threads.run(new ThreadUdpServer());

    threads.joinAll();

    web.stop();
    web_thread.join();
}

void AppAuth::initLogger() {

    logger.setLogGroupingInterval(conf.log_grouping_interval);

    logger.addLogWriter(pLogWriter(new LogWriterScreen()));

    if (!conf.log_file_filename.empty())
        logger.addLogWriter(pLogWriter(new LogWriterFile(conf.log_file_filename, conf.log_file_min_level, conf.log_file_max_level)));

    if (!conf.log_syslog_ident.empty())
        logger.addLogWriter(pLogWriter(new LogWriterSyslog(conf.log_syslog_ident, conf.log_syslog_min_level, conf.log_syslog_max_level)));

}

shared_ptr<ConfigVersionData> AppAuth::getConfigVersionData() {
    lock_guard<mutex> lock(configVersionDataMutex);

    shared_ptr<ConfigVersionData> tmp = configVersionData;

    return tmp;
}

void AppAuth::setConfigVersionData(ConfigVersionData * configVersionData) {
    lock_guard<mutex> lock(configVersionDataMutex);

    shared_ptr<ConfigVersionData> tmp(configVersionData);

    this->configVersionData.swap(tmp);
}