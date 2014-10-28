#pragma once

#include <mutex>
#include "../../src/classes/App.h"
#include "ConfAuth.h"
#include "ConfigVersionData.h"

class AppAuth : public App {
public:
    ConfAuth conf;

    // флаг устанавливается когда первоначальное получение данных с центрального сервера завершено
    volatile bool init_sync_done;
    // флаг устанавливается когда счетчики посчитаны и загружены в память
    volatile bool init_load_counters_done;
    // флаг устанавливается когда справочники загружены в память
    volatile bool init_load_data_done;
    // флаг устанавливается когда обсчитаны все исторические данные и биллинг перешел в режим ожидания новых данных
    volatile bool init_bill_runtime_started;

    AppAuth();
    virtual bool init(int argc, char* argv[]) override;
    shared_ptr<ConfigVersionData> getConfigVersionData();
    void setConfigVersionData(ConfigVersionData * configVersionData);
protected:
    shared_ptr<ConfigVersionData> configVersionData;
    mutex configVersionDataMutex;
    void runApp();
    void initLogger();
    
};

AppAuth & app();


