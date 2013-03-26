#pragma once

#include "Obj.h"
#include "ObjList.h"

class PriceObjList : public ObjListByIntPrefix {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key0(void *obj);
    char * key(void *obj);
public:
    pPriceObj find(int pricelist_id, char * prefix);
};
