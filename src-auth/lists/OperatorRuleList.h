#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/OperatorRule.h"

class OperatorRuleList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pOperatorRule find(const int operator_id, const bool outgoing, const int order);
};
