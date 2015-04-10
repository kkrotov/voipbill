#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/obj.h"

class PriceObjList : public ObjListByIntPrefix {
protected:

    size_t item_size() {
        return sizeof (PriceObj);
    }

    string sql(BDb * db) {
        string time = string_date(dt);
        return "	select pricelist_id, ndef::varchar, cast(price*10000 as integer) from billing.defs " \
            "	where date_from <= '" + time + "' and (date_to is null or date_to >= '" + time + "') and deleted=false" \
            "	order by pricelist_id, ndef::varchar";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        pPriceObj item = (pPriceObj) obj;
        item->pricelist_id = row.get_i(0);
        memcpy(item->prefix, row.get(1), 21);
        item->price = row.get_i(2);
    }

    inline int key0(const void *obj) {
        return ( (pPriceObj) obj)->pricelist_id;
    }

    inline char * key(const void *obj) {
        return ( (pPriceObj) obj)->prefix;
    }


public:
    pPriceObj find(const int pricelist_id, const char * prefix) {
        return (pPriceObj) _find(pricelist_id, prefix);
    }

    pPriceObj find(const int pricelist_id, const long long int nPrefix) {
        char prefix[20];
        sprintf(prefix, "%lld", nPrefix);
        return (pPriceObj) _find(pricelist_id, prefix);
    }
};
