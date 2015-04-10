#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/GeoPrefix.h"

class GeoPrefixList : public ObjListByPrefix {
protected:
    size_t item_size() {
        return sizeof (GeoPrefix);
    }

    string sql(BDb * db) {
        return "   select prefix, dest, geo_id, operator_id, mob" \
                "   from geo.prefix " \
                "   order by prefix asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        GeoPrefix * item = (GeoPrefix *) obj;
        strcpy(item->prefix, row.get(0));
        item->dest = row.get_i(1);
        item->geo_id = row.get_i(2);
        item->geo_operator_id = row.get_i(3);
        item->mob = row.get_b(4);
    }

    inline char * key(const void *obj) {
        return ( (GeoPrefix *) obj)->prefix;
    }


public:

    GeoPrefix * find(const char * prefix) {
        return (GeoPrefix *) _find(prefix);
    }
};
