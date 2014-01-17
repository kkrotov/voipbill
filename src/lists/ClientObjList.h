#pragma once

#include "Obj.h"
#include "ObjList.h"

class ClientObjList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pClientObj find(const int client_id);
};
