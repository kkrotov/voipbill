#pragma once

#include "Task.h"
#include "../CalcFull.h"

class TaskBillRuntime : public Task, public CallsObjList
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
    TaskBillRuntime();
};

