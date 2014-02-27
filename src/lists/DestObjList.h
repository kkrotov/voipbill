#pragma once

#include "ObjList.h"

class DestObjList : public ObjListByPrefix {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    char * key(const void *obj);
public:
    pDestObj find(const char * prefix);
};
