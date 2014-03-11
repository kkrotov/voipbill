#pragma once

#include "../../src/classes/Timer.h"
#include "../../src/lists/RuntimeCallsObjList.h"
#include "../../src/classes/CalcFull.h"
#include "../../src/tasks/Task.h"

class TaskRecalc : public Task {
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

