#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/obj.h"

class PriceObjList : public ObjListByIntPrefix {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key0(const void *obj);
    char * key(const void *obj);
public:
    pPriceObj find(const int pricelist_id, const char * prefix);
    pPriceObj find(const int pricelist_id, const long long int nPrefix);
};
