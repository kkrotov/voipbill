#pragma once

#include "Obj.h"
#include "ObjList.h"

class AsteriskNumberObjList : public ObjListBy2Long {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    long long int key(void *obj);
public:
    pAsteriskNumberObj find(long long int value);
};
