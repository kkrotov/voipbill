#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Number.h"

class NumberList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pNumber find(const int id);
};
