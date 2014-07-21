#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Prefixlist.h"

class PrefixlistList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pPrefixlist find(const int id);
};
