#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/obj.h"

class GlobalCountersObjList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pGlobalCountersObj find(const int client_id);
};
