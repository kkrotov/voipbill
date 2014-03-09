#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/CalcFull.h"
#include "../../src/classes/CallsSaver.h"
#include "../../src/lists/RuntimeCallsObjList.h"

class ThreadBillRuntime : public Thread {
protected:
    BDb db_rad;
    BDb db_calls;
    CalcFull calculator;
    CallsSaver calls_saver;

    RuntimeCallsObjList calls_list;

    int calc_calls_loop;
    int calc_calls_full;

    Timer t_calc;
    Timer t_save;

public:
    static bool need_refresh_current_id;

    bool ready();

    void run();

    void htmlfull(stringstream &html);
public:
    ThreadBillRuntime();
};

