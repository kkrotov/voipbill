#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/PricelistPrice.h"

class PricelistPriceList : public ObjListByIntLongPeriod {
protected:

    size_t item_size() {
        return sizeof (PriceObj);
    }

    string sql(BDb * db) {
        return "	select pricelist_id, ndef, extract(epoch from date_from), extract(epoch from date_to), ndef, cast(price*10000 as integer),  " \
                "   from billing.defs " \
                "	where deleted=false and date_to < now() " \
                "	order by pricelist_id, ndef, date_from ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        PricelistPrice * item = (PricelistPrice *) obj;
        item->pricelist_id = row.get_i(0);
        item->prefix = row.get_ll(1);
        item->price = row.get_i(2);
        item->date_from = row.get_ll(3);
        item->date_to = row.get_ll(4);
    }

    inline int key1(const void *obj) {
        return ( (PricelistPrice *) obj)->pricelist_id;
    }

    inline long long int key2(const void *obj) {
        return ( (PricelistPrice *) obj)->prefix;
    }

    inline time_t key3_from(const void *obj) {
        return ( (PricelistPrice *) obj)->date_from;
    }

    inline time_t key3_to(const void *obj) {
        return ( (PricelistPrice *) obj)->date_to;
    }

public:
    PricelistPrice * find(const int pricelist_id, const char * prefix, time_t timestamp) {
        char tmpPrefix[20];
        strcpy(tmpPrefix, prefix);
        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            long long int nPrefix = atoll(prefix);
            void * result = _find(pricelist_id, prefix, timestamp);
            if (result != 0) {
                return (PricelistPrice *) result;
            }

            len -= 1;
        }
        return (PricelistPrice *) 0;
    }

    PricelistPrice * find(const int pricelist_id, const long long int nPrefix, time_t timestamp) {
        char prefix[20];
        sprintf(prefix, "%lld", nPrefix);

        return (PricelistPrice *) _find(pricelist_id, prefix, timestamp);
    }

};
