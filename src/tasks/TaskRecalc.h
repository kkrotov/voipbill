#pragma once

#include "../classes/Timer.h"
#include "../lists/RuntimeCallsObjList.h"
#include "../classes/CalcFull.h"
#include "Task.h"

class TaskRecalc : public Task
{
protected:
    time_t date_from;

    RuntimeCallsObjList calls_list;

    int calc_calls_loop;
    int calc_calls_full;

    CalcFull calculator;

    Timer t_calc;
    Timer t_save;

    void html(stringstream &html);
public:
    TaskRecalc(time_t date_from);

    void run();
};

