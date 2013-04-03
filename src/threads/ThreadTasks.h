#pragma once

#include "Thread.h"
#include "../classes/CalcFull.h"

class ThreadTasks : public Thread
{
protected:
    Timer t;

    void run();

    void htmlfull(stringstream &html);

public:
    ThreadTasks();
};

