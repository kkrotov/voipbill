#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Tariff.h"

class TariffList : public ObjListByInt<Tariff> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, freemin, freemin_for_number, pricelist_id, paid_redirect, tariffication_by_minutes, tariffication_full_first_minute, tariffication_free_first_seconds " \
            "   from billing.tarif " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Tariff * item) {
        item->id = row.get_i(0);
        item->freemin = row.get_i(1);
        item->freemin_for_number = row.get_b(2);
        item->pricelist_id = row.get_i(3);
        item->paid_redirect = row.get_b(4);
        item->tariffication_by_minutes = row.get_b(5);
        item->tariffication_full_first_minute = row.get_b(6);
        item->tariffication_free_first_seconds = row.get_b(7);
    }

    inline int key(Tariff *item) {
        return item->id;
    }
public:
    Tariff * find(const int id) {
        return (Tariff *) _find(id);
    }
};
