#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/Timer.h"
#include "../classes/Repository.h"

class ThreadLoader : public Thread {
    Repository repository;
    BDb db_calls;
    string current_event;

    // флаг устанавливается когда счетчики посчитаны и загружены в память
    bool init_load_counters_done;
    // флаг устанавливается когда справочники загружены в память
    bool init_load_data_done;

    bool prepare();
    void run();


public:
    virtual bool hasFullHtml() override;

    virtual void htmlfull(stringstream &html) override;

public:

    bool do_load_data();

    bool do_load_counters();

    ThreadLoader();
    static const char* idName() { return "loader"; }
};
