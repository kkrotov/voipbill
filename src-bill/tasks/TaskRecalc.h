#pragma once

#include "../../src/classes/Timer.h"
#include "../lists/CdrObjList.h"
#include "../classes/CalcFull.h"
#include "../../src/tasks/Task.h"

class TaskRecalc : public Task {
protected:
    time_t date_from;

    CdrObjList cdr_list;

    int calc_calls_loop;
    int calc_calls_full;

    CalcFull calculator;

    Timer t_calc;
    Timer t_save;

    void html(stringstream &html);
public:
    TaskRecalc(time_t date_from, string server_id);

    void run();
};

