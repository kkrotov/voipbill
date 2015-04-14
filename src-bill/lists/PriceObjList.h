#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/PriceObj.h"

class PriceObjList : public ObjListByIntPrefix<PriceObj> {
protected:

    string sql(BDb * db) {
        string time = string_date(dt);
        return "	select pricelist_id, ndef::varchar, cast(price*10000 as integer) from billing.defs " \
            "	where date_from <= '" + time + "' and (date_to is null or date_to >= '" + time + "') and deleted=false" \
            "	order by pricelist_id, ndef::varchar";
    }

    inline void parse_item(BDbResult &row, PriceObj * item) {
        item->pricelist_id = row.get_i(0);
        memcpy(item->prefix, row.get(1), 21);
        item->price = row.get_i(2);
    }

    inline int key0(PriceObj *item) {
        return item->pricelist_id;
    }

    inline char * key(PriceObj *item) {
        return item->prefix;
    }


public:
    PriceObj * find(const int pricelist_id, const char * prefix) {
        return (PriceObj *) _find(pricelist_id, prefix);
    }

    PriceObj * find(const int pricelist_id, const long long int nPrefix) {
        char prefix[20];
        sprintf(prefix, "%lld", nPrefix);
        return (PriceObj *) _find(pricelist_id, prefix);
    }
};
