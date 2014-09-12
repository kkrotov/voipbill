#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/RoutingReportData.h"

class RoutingReportDataList : public ObjListByPrefix {
protected:
    size_t item_size();
    string sql(BDb * db);
    void parse_item(BDbResult &row, void * obj);
    char * key(const void *obj);
public:
    pRoutingReportData find(const char * prefix);
};
