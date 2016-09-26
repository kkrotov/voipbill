#pragma once

#include "../classes/BDb.h"
#include "../models/Call.h"
#include "Spinlock.h"

#define CALLS_PARTITION_SIZE 50000
#define CALLS_MAX_QUEUE_SIZE 1000000

class CallsManager {
private:
    vector<vector<Call>> realtimeCallsParts;

    long long int realtimeLastId;
    long long int storedLastId;

    time_t realtimeLastTime;
    time_t storedLastTime;

    size_t realtimeCounter;
    size_t storedCounter;

public:
    CallsManager();

    void clear();

    void prepareSaveQueries(map<time_t, stringstream> &queryPerMonth, vector<string> &querytime);

    void executeSaveQueries(BDb *dbCalls, map<time_t, stringstream> &queryPerMonth);

    void createIfNotExists(BDb *dbCalls, string qtime);

private:
    bool ready();

    void add(Call &call);

    void add(Call &origCall, Call &termCall);

    size_t getQueueSize();

    long long int getLastId();

    time_t getLastTime();

    size_t getCounter();

    long long int getStoredLastId();

    time_t getStoredLastTime();

    size_t getStoredCounter();

    void setLastId(long long int lastId);

    void setLastTime(time_t lastTime);

    void setStoredLastId(long long int lastId);

    void setStoredLastTime(time_t lastTime);


    bool isNeedSave();

    bool isNeedCreatePartitionBeforeSave();

    bool isNeedCreatePartitionAfterAdd();

    void createNewPartition();

    void removePartitionAfterSave();

    size_t getLastRealtimePartSize();

    void calls_insert_row(Call *call, stringstream &q);


    friend class DataBillingContainer;

    friend class Billing;
};
