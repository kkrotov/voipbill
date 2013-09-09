#pragma once

#include "Thread.h"
#include "../classes/BDb.h"

class ThreadCheckStartTable : public Thread {
    BDb db_rad;
    time_t last_t;

    void run();

    void htmlfull(stringstream &html);

public:
    ThreadCheckStartTable();
};

