#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/CalcFull.h"

class ThreadLimitControl : public Thread {
protected:
    Timer t_calc;
    Timer t_kill;
    BDb db_calls;
    CalcFull calculator;

    bool ready();

    void run();

    void htmlfull(stringstream &html);

public:
    ThreadLimitControl();
};

