#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/obj.h"

class CurrentCallsObjList : public ObjListBy2Long {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    long long int key(const void *obj);
public:
    pCallObj get(size_t i);
    pCallObj find(const long long int value);
};
