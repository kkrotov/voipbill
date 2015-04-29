#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"

class ThreadCheckStartTable : public Thread {
    BDb db_calls;
    time_t last_t;
    int countWaitingForFinishCalls;
    int countForceFinishedCalls;

    map<string, bool> radacctCalls;
    map<string, bool> opencaCalls;
    map<string, string> waitingForFinishCalls;

    void run();

    void htmlfull(stringstream &html);

    void readRadacctCalls();
    void readOpencaCalls();
    void forceFinishCall(string callId);

public:
    ThreadCheckStartTable();
    static const char* idName() { return "checkstarttable"; }
};

