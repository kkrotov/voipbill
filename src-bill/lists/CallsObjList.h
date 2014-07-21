#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/obj.h"

class CallsObjList : public ObjList {
protected:
    size_t item_size();
public:
    long long int last_id;
    long long int new_last_id;
    bool loaddata(BDb *db);
    bool next();
    pCallObj get(size_t i);
    CallsObjList();
};
