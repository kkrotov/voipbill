#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/Spinlock.h"
#include "../lists/RegionsCountersObjList.h"

class ThreadRegionsCounters : public Thread {
public:
    static shared_ptr<RegionsCountersObjList> getList();
private:
    static Spinlock sync_mutex;
    static shared_ptr<RegionsCountersObjList> list;
    static void setList(shared_ptr<RegionsCountersObjList> plist);

    BDb db_main;

    void run();


    void htmlfull(stringstream &html);

public:
    ThreadRegionsCounters();
};

