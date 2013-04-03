#pragma once

#include "Thread.h"

class ThreadCheckStartTable : public Thread
{
    Timer t;

    void run();

    void htmlfull(stringstream &html);

public:
    ThreadCheckStartTable();
};

