#pragma once

#include "BasePull.h"

#include <map>

#include "../../src/classes/BDb.h"

class BasePull;

class ManagerPull {
public:
    BDb db_main;
    BDb db_calls;
    map<string, BasePull *> pulls;
    int part_size;
    int errors_count;

    ManagerPull();
    void add(BasePull * pull);
    void pull();

    static ManagerPull * instance() {
        static ManagerPull inst;
        return &inst;
    }

protected:
    void clearPulls();
    void runPulls();
};

