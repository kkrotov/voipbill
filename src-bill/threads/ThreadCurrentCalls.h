#pragma once

#include "../../src/threads/Thread.h"
#include "../lists/CurrentCallsObjList.h"

#include <mutex>

class ThreadCurrentCalls : public Thread {
public:
    static shared_ptr<CurrentCallsObjList> getList();
private:
    static std::mutex sync_mutex;
    static shared_ptr<CurrentCallsObjList> list;
    static void setList(shared_ptr<CurrentCallsObjList> plist);

    BDb db_rad;

    void run();


    void htmlfull(stringstream &html);

public:
    ThreadCurrentCalls();
};

