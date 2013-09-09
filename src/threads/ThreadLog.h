#pragma once

#include "../threads/Thread.h"

class ThreadLog : public Thread {
private:
    Timer t;

    void run();

    void htmlfull(stringstream &html);

public:
    ThreadLog();
};

