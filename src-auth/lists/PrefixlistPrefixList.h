#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/PrefixlistPrefix.h"

class PrefixlistPrefixList : public ObjListByIntPrefix {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key0(const void *obj);
    char * key(const void *obj);
public:
    pPrefixlistPrefix find(const int prefixlist_id, const char * prefix);
    pPrefixlistPrefix find(const int prefixlist_id, const long long int nPrefix);
};
