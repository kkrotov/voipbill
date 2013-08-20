#pragma once

#include "Obj.h"
#include "ObjList.h"

class CurrentCallsObjList : public ObjListBy2Long {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    long long int key(void *obj);
public:
    pCallObj get(size_t i);
    pCallObj find(long long int value);
};
