#pragma once

#include "Task.h"
#include "../classes/CalcFull.h"

class TaskLimitControl : public Task
{
protected:
    Timer t;
    Timer t_calc;
    Timer t_kill;
    CalcFull calculator;

    void wait();

    void run();

    void htmlfull(stringstream &html);

public:
    TaskLimitControl();
};

