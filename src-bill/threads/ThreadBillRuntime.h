#pragma once

#include "../../src/threads/Thread.h"
#include "../classes/CalcFull.h"
#include "../classes/CallsSaver.h"
#include "../lists/CdrObjList.h"

class ThreadBillRuntime : public Thread {
protected:
    BDb db_calls;
    CalcFull calculator;
    CallsSaver calls_saver;

    CdrObjList cdr_list;

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
    static const char* idName() { return "runtime"; }
};

