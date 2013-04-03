#pragma once

#include "Thread.h"
#include "../classes/CalcFull.h"

class ThreadBillRuntime : public Thread, public CallsObjList
{
protected:
    BDb db_rad;
    BDb db_calls;
    CalcFull calculator;
    int calc_calls_loop;
    int calc_calls_full;

    Timer t;
    Timer t_calc;
    Timer t_save;

    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);

public:

    void wait();

    void run();

    void htmlfull(stringstream &html);
public:
    ThreadBillRuntime();
};

