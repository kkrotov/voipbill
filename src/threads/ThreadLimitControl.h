#pragma once

#include "Thread.h"
#include "../classes/CalcFull.h"

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

