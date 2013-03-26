#pragma once

#include "Obj.h"
#include "ObjList.h"

class OperatorList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(void *obj);
public:
    pOperator find(const int id);
};
