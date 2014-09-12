#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Operator.h"

class OperatorList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pOperator find(const int id);
};
