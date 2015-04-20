#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/PricelistPrice.h"

class PricelistPriceList : public ObjList<PricelistPrice> {
protected:

    string sql(BDb * db) {
        return "	select pricelist_id, ndef, price, extract(epoch from date_from), extract(epoch from date_to) " \
                "   from billing.defs " \
                "	where deleted=false and date_to > now() " \
                "	order by pricelist_id asc, ndef::varchar asc, date_from asc ";
    }

    inline void parse_item(BDbResult &row, PricelistPrice * item) {
        item->pricelist_id = row.get_i(0);
        strcpy(item->prefix, row.get(1));
        item->price = row.get_d(2);
        item->date_from = row.get_ll(3);
        item->date_to = row.get_ll(4);
    }

    struct key_pricelist_id {
        bool operator() (const PricelistPrice & left, int pricelist_id) {
            return left.pricelist_id < pricelist_id;
        }
        bool operator() (int pricelist_id, const PricelistPrice & right) {
            return pricelist_id < right.pricelist_id;
        }
    };

    struct key_prefix {
        bool operator() (const PricelistPrice & left, char * prefix) {
            return strcmp(left.prefix, prefix) < 0;
        }
        bool operator() (char * prefix, const PricelistPrice & right) {
            return strcmp(prefix, right.prefix) < 0;
        }
    };

    struct key_timestamp {
        bool operator() (const PricelistPrice & left, time_t timestamp) {
            return left.date_to < timestamp;
        }
        bool operator() (time_t timestamp, const PricelistPrice & right) {
            return timestamp < right.date_from;
        }
    };

    PricelistPrice * _find(int pricelist_id, char * prefix, time_t timestamp) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, pricelist_id, key_pricelist_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, prefix, key_prefix());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }

public:
    PricelistPrice * find(int pricelist_id, char * prefix, time_t timestamp) {
        char tmpPrefix[20];
        strcpy(tmpPrefix, prefix);
        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            auto result = _find(pricelist_id, tmpPrefix, timestamp);
            if (result != nullptr) {
                return result;
            }

            len -= 1;
        }
        return nullptr;
    }

};
