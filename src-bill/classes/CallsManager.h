#pragma once

#include "../models/Call.h"
#include "Spinlock.h"

class CallsManager
{
private:
    Spinlock lock;
    deque<Call> queue;

    long long int lastId;
    time_t lastTime;
    size_t counter;

    long long int storedLastId;
    time_t storedLastTime;
    size_t storedCounter;

public:
    CallsManager();
    bool ready();

    void add(Call &origCall, Call &termCall);
    void get(vector<Call> &calls, size_t maxCount);
    void revert(vector<Call> &calls);
    size_t size();

    long long int getLastId();
    time_t getLastTime();
    size_t getCounter();
    void setLastId(long long int lastId);
    void setLastTime(time_t lastTime);

    long long int getStoredLastId();
    time_t getStoredLastTime();
    size_t getStoredCounter();
    void setStoredLastId(long long int lastId);
    void setStoredLastTime(time_t lastTime);
    void incStoredCounter(size_t count);
};
