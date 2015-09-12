#pragma once

#include "../models/Cdr.h"
#include "Spinlock.h"

#define CDRS_PARTITION_SIZE 25000

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

    void add(const Cdr &cdr);
    bool get(Cdr &cdr);
    void revert(const Cdr &cdr);

private:
    bool ready();
    void setLastId(long long int lastId);
    void setLastTime(time_t lastTime);

    size_t getLastPartSize();

    friend class DataBillingContainer;
};
