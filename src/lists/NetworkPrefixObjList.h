#pragma once

#include "ObjList.h"

class NetworkPrefixObjList : public ObjListByIntPrefix {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key0(const void *obj);
    char * key(const void *obj);
public:
    pNetworkPrefixObj find(const int operator_id, const char * prefix);
};
