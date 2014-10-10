#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"

class ThreadCheckStartTable : public Thread {
    BDb db_rad;
    time_t last_t;
    int countWaitingForFinishCalls;
    int countForceFinishedCalls;

    map<string, string> radacctCalls;
    map<string, bool> opencaCalls;
    map<string, string> waitingForFinishCalls;

    void run();

    void htmlfull(stringstream &html);

    void readRadacctCalls();
    void readOpencaCalls();
    void forceFinishCall(string acctSessionId, string acctUniqueId);

public:
    ThreadCheckStartTable();
    static const char* idName() { return "checkstarttable"; }
};

