#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../data/DataCurrentCallsContainer.h"

class ThreadCheckStartTable : public Thread {
    BDb db_calls;

    int countWaitingForFinishCalls;
    int countForceFinishedCalls;

    void run();

    void htmlfull(stringstream &html);

    void fetchOpencaCurrentCalls(set<long long int> &opencaCalls);
    void spawnWaitingForFinishCalls(set<long long int> &waitingForFinishCalls, set<long long int> &opencaCalls, CurrentCdrList * cdrList);
    void forceFinishCalls(set<long long int> &waitingForFinishCalls);


public:
    bool hasFullHtml() override;
    ThreadCheckStartTable();
    static const char* idName() { return "checkstarttable"; }
};

