#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/GeoPrefix.h"

class GeoPrefixList : public ObjListByPrefix<GeoPrefix> {
protected:

    string sql(BDb * db) {
        return "   select prefix, dest, geo_id, operator_id, region, mob" \
                "   from geo.prefix " \
                "   order by prefix asc ";
    }

    inline void parse_item(BDbResult &row, GeoPrefix * item) {
        strcpy(item->prefix, row.get(0));
        item->dest = row.get_i(1);
        item->geo_id = row.get_i(2);
        item->geo_operator_id = row.get_i(3);
        item->region_id = row.get_i(4);
        item->mob = row.get_b(5);
    }

    inline char * key(GeoPrefix *item) {
        return item->prefix;
    }


public:

    GeoPrefix * find(const char * prefix) {
        return (GeoPrefix *) _find(prefix);
    }
};
