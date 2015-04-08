#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/obj.h"

class CdrObjList : public ObjList {
protected:
    size_t item_size();    
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
public:
    long long int last_id;
    long long int new_last_id;
    const int nrows = 50000;
    bool next();
    bool loaddata(BDb *db);
    pCdrObj get(size_t i);
    CdrObjList();
    void refresh_current_id();
};
