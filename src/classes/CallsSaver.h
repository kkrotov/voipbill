#pragma once

#include "../lists/CallsObjList.h"

class CallsSaver
{
protected:
    BDb * db;

public:
    CallsSaver(BDb *db);

    void update(CallsObjList *list);
    void save(CallsObjList *list);

};

