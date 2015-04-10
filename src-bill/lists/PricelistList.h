#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Pricelist.h"

class PricelistList : public ObjListByInt {
protected:

    size_t item_size() {
        return sizeof (Pricelist);
    }

    string sql(BDb * db) {
        return "   select  id, region, operator_id, tariffication_by_minutes, tariffication_full_first_minute " \
            "   from billing.pricelist " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        Pricelist * item = (Pricelist *) obj;
        item->id = row.get_i(0);
        item->region = row.get_i(1);
        item->operator_id = row.get_i(2);
        item->tariffication_by_minutes = row.get_b(3);
        item->tariffication_full_first_minute = row.get_b(4);
    }

    inline int key(const void *obj) {
        return ((Pricelist *) obj)->id;
    }


public:
    Pricelist * find(const int id) {
        return (Pricelist *) _find(id);
    }
};
