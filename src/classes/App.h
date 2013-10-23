#pragma once

#include "Conf.h"
#include "Logger.h"
#include "ThreadPool.h"

#include <boost/signals2.hpp>

enum AppStatus {
    APP_STARTED, APP_INITIALIZING, APP_PREPARING, APP_RUNNING, APP_PAUSED, APP_STOPPED
};

static const char * AppStatusNames[]{
    "STARTED", "INITIALIZING", "PREPARING", "RUNNING", "PAUSED", "STOPPED"
};

class App {
public:
    Conf conf;
    Logger logger;
    ThreadPool threads;

    AppStatus status;
    AppStatus real_status;

    // флаг устанавливается когда первоначальное получение данных с центрального сервера завершено
    volatile bool init_sync_done;
    // флаг устанавливается когда счетчики посчитаны и загружены в память
    volatile bool init_load_counters_done;
    // флаг устанавливается когда справочники загружены в память
    volatile bool init_load_data_done;
    // флаг устанавливается когда обсчитаны все исторические данные и биллинг перешел в режим ожидания новых данных
    volatile bool init_bill_runtime_started;

    mutex bill_runnning_mutex;

    boost::signals2::signal<void() > onStatusChanged;
    boost::signals2::signal<void() > onRealStatusChanged;

    App();

    bool init(int argc, char* argv[]);
    void run();

    void setStatus(AppStatus status);
    void setRealStatus(AppStatus real_status);
    AppStatus getStatus();
    AppStatus getRealStatus();

protected:
    void initLogger();
};

extern App app;


