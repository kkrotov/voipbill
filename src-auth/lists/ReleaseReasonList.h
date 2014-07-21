#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ReleaseReason.h"

class ReleaseReasonList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pReleaseReason find(const int id);
};
