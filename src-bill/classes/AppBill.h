#pragma once

#include "../../src/classes/App.h"

class AppBill : public App {
public:
    // флаг устанавливается когда первоначальное получение данных с центрального сервера завершено
    volatile bool init_sync_done;
    // флаг устанавливается когда счетчики посчитаны и загружены в память
    volatile bool init_load_counters_done;
    // флаг устанавливается когда справочники загружены в память
    volatile bool init_load_data_done;
    // флаг устанавливается когда обсчитаны все исторические данные и биллинг перешел в режим ожидания новых данных
    volatile bool init_bill_runtime_started;

    AppBill();

protected:
    void runApp();
    void initLogger();
};

AppBill & appBill();


