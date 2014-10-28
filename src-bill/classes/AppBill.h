#pragma once

#include "../../src/classes/App.h"
#include "ConfBill.h"

class AppBill : public App {
public:
    ConfBill conf;

    // флаг устанавливается когда первоначальное получение данных с центрального сервера завершено
    volatile bool init_sync_done;
    // флаг устанавливается когда счетчики посчитаны и загружены в память
    volatile bool init_load_counters_done;
    // флаг устанавливается когда справочники загружены в память
    volatile bool init_load_data_done;
    // флаг устанавливается когда обсчитаны все исторические данные и биллинг перешел в режим ожидания новых данных
    volatile bool init_bill_runtime_started;

    AppBill();
    virtual bool init(int argc, char* argv[]) override;

    Thread * newThreadObject(std::string id);

protected:
    void runApp();
    void initLogger();
    void runAppInSingleMode();
    void runAppInTestMode();

    void registerAllThreads();    
    template<class T> void registerThread();

    std::map<std::string, Thread * (*)()> threadConstructorsMap;
};

AppBill & app();


