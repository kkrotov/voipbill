#pragma once

#include "../classes/Repository.h"

class ThreadHealthManager : public Thread {

    std::map<std::string, time_t> trunk_error_time;
    int trunk_critical_load_delay;

protected:
    Repository repository;

public:
    bool ready() { return repository.prepare(); };
    void run();

public:
    ThreadHealthManager();
    static const char* idName() { return "health_manager"; }
    void log_report(SystemStatus trunkLoadStatus, time_t new_critical_err_time, int err_delay);
};
