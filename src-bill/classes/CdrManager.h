#pragma once

#include "../models/Cdr.h"
#include "Spinlock.h"

class CdrManager
{
private:
    Spinlock lock;
    deque<Cdr> queue;

    long long int lastId;
    time_t lastTime;
    size_t counter;

public:
    CdrManager();
    bool ready();

    void add(const Cdr &cdr);
    bool get(Cdr &cdr);
    void revert(const Cdr &cdr);
    size_t size();

    long long int getLastId();
    time_t getLastTime();
    size_t getCounter();
    void setLastId(long long int lastId);
    void setLastTime(time_t lastTime);
};
