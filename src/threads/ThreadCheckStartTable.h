#pragma once

#include "Task.h"

class TaskCheckStartTable : public Task
{
    Timer t;

    void run();

    void htmlfull(stringstream &html);

public:
    TaskCheckStartTable();
};

