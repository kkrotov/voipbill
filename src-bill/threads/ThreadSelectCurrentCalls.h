#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/Spinlock.h"
#include "../lists/CurrentCallsObjList.h"

class ThreadSelectCurrentCalls : public Thread {
public:
    static shared_ptr<CurrentCallsObjList> getList();
private:
    static Spinlock sync_mutex;
    static shared_ptr<CurrentCallsObjList> list;
    static void setList(shared_ptr<CurrentCallsObjList> plist);

    BDb db_rad;

    void run();


    void htmlfull(stringstream &html);

public:
    ThreadSelectCurrentCalls();
    static const char* idName() { return "currentcalls"; }
};

