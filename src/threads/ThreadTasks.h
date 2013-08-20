#pragma once

#include "Thread.h"
#include "../tasks/Task.h"
#include "../classes/CalcFull.h"

class ThreadTasks : public Thread {
protected:
    Timer t;

    int tasks_count;

    shared_ptr<Task> current_task;

    void wait();

    void run();

    void htmlfull(stringstream &html);

public:
    ThreadTasks();
};

