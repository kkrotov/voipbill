#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Airp.h"

class AirpList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pAirp find(const int id);
};
