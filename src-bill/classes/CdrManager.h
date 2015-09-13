#pragma once

#include "../models/Cdr.h"
#include "Spinlock.h"
#include "BDb.h"

#define CDRS_PARTITION_SIZE 25000
#define CDRS_MAX_QUEUE_SIZE 50000

class CdrManager
{
private:
    Spinlock lock;
    vector<vector<Cdr>> cdrsParts;
    deque<Cdr> queue;

    long long int lastId;
    time_t lastTime;
    size_t counter;

public:
    CdrManager();

    bool ready();
    bool loadPart(BDb * db_calls);
    size_t getQueueSize();
    long long int getLastId();
    time_t getLastTime();
    size_t getCounter();
    Cdr * getFirstCdr();
    void removeFirstCdr();
    void setLastId(long long int lastId);
    void setLastTime(time_t lastTime);
};
