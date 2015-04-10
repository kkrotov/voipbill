#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/RoutingReportData.h"

class RoutingReportDataList : public ObjListByPrefix {
protected:
    size_t item_size() {
        return sizeof (RoutingReportData);
    }

    string sql(BDb * db) {
        return "   select prefix, routes " \
            "   from auth.routing_report_data " \
            "   order by prefix asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        RoutingReportData * item = (RoutingReportData *) obj;
        row.fill_cs(0, item->prefix, sizeof(item->prefix));
        row.fill_cs(1, item->routes, sizeof(item->routes));
    }

    inline int key(const void *obj) {
        return ((RoutingReportData *) obj)->prefix;
    }
public:
    RoutingReportData * find(const char * prefix) {
        return (RoutingReportData *) _find(prefix);
    }
};
