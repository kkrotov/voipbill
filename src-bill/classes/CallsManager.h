#pragma once

#include "../classes/BDb.h"
#include "../models/Call.h"
#include "Spinlock.h"

class CallsManager
{
private:
    Spinlock lock;
    deque<Call> realtimeQueue;
    vector<Call> tmpQueue;

    long long int realtimeLastId;
    long long int tmpLastId;
    long long int storedLastId;

    time_t realtimeLastTime;
    time_t tmpLastTime;
    time_t storedLastTime;

    size_t realtimeCounter;
    size_t tmpCounter;
    size_t storedCounter;

public:
    CallsManager();
    bool ready();

    void add(Call &origCall, Call &termCall);
    void get(vector<Call> &calls, size_t maxCount);
    void revert(vector<Call> &calls);
    size_t size();

    void save(BDb * dbCalls);
    void moveRealtimeToTemp();
    void moveTempToStored();

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
