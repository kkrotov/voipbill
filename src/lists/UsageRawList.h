#pragma once

#include "Obj.h"
#include "ObjList.h"

class UsageRawList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(void *obj);
public:
    pUsageRaw find(const int id);
};
