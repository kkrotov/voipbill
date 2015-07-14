#pragma once

#include <set>
#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/Repository.h"


class ThreadCheckStartTable : public Thread {
    BDb db_calls;
    Repository repository;

    size_t countWaitingForFinishCalls;
    size_t countForceFinishedCalls;

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

