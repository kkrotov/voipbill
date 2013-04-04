#pragma once

#include "Thread.h"
#include "../classes/CalcFull.h"
#include "../lists/RuntimeCallsObjList.h"

class ThreadBillRuntime : public Thread
{
protected:
    BDb db_rad;
    BDb db_calls;
    CalcFull calculator;

    RuntimeCallsObjList calls_list;

    int calc_calls_loop;
    int calc_calls_full;

    Timer t;
    Timer t_calc;
    Timer t_save;

public:
    static bool need_refresh_current_id;

    void wait();

    void run();

    void htmlfull(stringstream &html);
public:
    ThreadBillRuntime();
};

