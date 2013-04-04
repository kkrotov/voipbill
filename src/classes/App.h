#pragma once

#include <list>
#include <mutex>
using namespace std;

#include "Conf.h"

class Thread;

class App
{
public:
    Conf conf;
    mutex threads_mutex;
    list<Thread*> threads;

    // флаг устанавливается когда первоначальное получение данных с центрального сервера завершено
    bool init_sync_done;
    // флаг устанавливается когда счетчики посчитаны и загружены в память
    bool init_load_counters_done;
    // флаг устанавливается когда справочники загружены в память
    bool init_load_data_done;
    // флаг устанавливается когда обсчитаны все исторические данные и биллинг перешел в режим ожидания новых данных
    bool init_bill_runtime_started;

    mutex bill_runnning_mutex;


    App();

    bool init(int argc, char* argv[]);
    void run();

    void register_thread(Thread * thread);
    void unregister_thread(Thread * thread);

};

extern App app;


