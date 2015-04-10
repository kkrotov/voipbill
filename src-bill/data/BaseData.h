#pragma once

#include <mutex>

#include "../../src/common.h"
#include "../../src/lists/ObjList.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Timer.h"

using namespace std;

class BaseData {
protected:
    shared_ptr<ObjList> data;
    mutex lock;
    virtual ObjList * create() = 0;
public:
    Timer timer;
    BaseData();
    bool ready();
    shared_ptr<ObjList> get();
    void load(BDb * db);
    time_t time();
    int rows();
    int size();
};