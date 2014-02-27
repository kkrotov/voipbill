#pragma once

#include "ObjList.h"

class UsageObjList : public ObjListBy2Long {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    long long int key(const void *obj);
public:
    pUsageObj find(const char * phone_num);
    pUsageObj find(const long long int phone_num);
};
