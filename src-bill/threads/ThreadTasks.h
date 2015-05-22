#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/tasks/Task.h"

class ThreadTasks : public Thread {
protected:
    BDb db_main;

    int tasks_count;

    shared_ptr<Task> current_task;

    bool ready();

    void run();

    void htmlfull(stringstream &html);


public:
    bool hasFullHtml() override;

public:
    ThreadTasks();
    static const char* idName() { return "tasks"; }
};

