#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/RouteTableRoute.h"

class RouteTableRouteList : public ObjListByInt {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    int key(const void *obj);
public:
    pRouteTableRoute find(const int route_table_id, const int order);
};
