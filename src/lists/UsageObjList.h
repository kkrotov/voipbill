#pragma once

#include "Obj.h"
#include "ObjList.h"

class UsageObjList : public ObjListBy2Long {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    long long int key(void *obj);
public:
    pUsageObj find(const char * phone);
    pUsageObj find(const long long int phone);
};
