#pragma once

#include "../lists/CallsObjList.h"

class CallsSaver {
protected:
    BDb * db;

public:
    CallsSaver();
    CallsSaver(BDb *db);

    void setDb(BDb *db);

    void update(CallsObjList *list);
    void save(CallsObjList *list);

};

