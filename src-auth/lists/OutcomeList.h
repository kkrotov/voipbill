#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Outcome.h"

class OutcomeList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pOutcome find(const int id);
};
