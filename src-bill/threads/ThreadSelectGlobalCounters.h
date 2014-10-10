#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/Spinlock.h"
#include "../lists/GlobalCountersObjList.h"

class ThreadSelectGlobalCounters : public Thread {
public:
    static shared_ptr<GlobalCountersObjList> getList();
private:
    static Spinlock sync_mutex;
    static shared_ptr<GlobalCountersObjList> list;
    static void setList(shared_ptr<GlobalCountersObjList> plist);

    BDb db_main;

    void run();


    void htmlfull(stringstream &html);

public:
    ThreadSelectGlobalCounters();
    static const char* idName() { return "select_global_counters"; }
};

