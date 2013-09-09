#pragma once

#include "Thread.h"
#include "../classes/CalcFull.h"

class ThreadLimitControl : public Thread {
protected:
    Timer t;
    Timer t_calc;
    Timer t_kill;
    CalcFull calculator;

    bool ready();

    void run();

    void htmlfull(stringstream &html);

public:
    ThreadLimitControl();
};

