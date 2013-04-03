#pragma once

#include "Task.h"
#include "../classes/CalcFull.h"

class TaskTasks : public Task
{
protected:
    Timer t;

    void run();

    void htmlfull(stringstream &html);

public:
    TaskTasks();
};

