#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Client.h"

class ClientList : public ObjListByInt {
protected:
    size_t item_size() {
        return sizeof (Client);
    }

    string sql(BDb * db) {
        return "   select id, voip_limit_month, voip_limit_day, credit, balance, amount_date, last_payed_month, voip_disabled " \
                "   from billing.clients " \
                "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        pClient item = (pClient) obj;
        item->id = row.get_i(0);
        item->limit_m = row.get_i(1);
        item->limit_d = row.get_i(2);
        item->credit = row.get_i(3);
        item->balance = row.get_d(4);
        item->amount_date = parseDateTime(row.get(5));
        item->last_payed_month = parseDate(row.get(6));
        item->disabled = row.get_b(7);
    }

    inline int key(const void *obj) {
        return ( (pClient) obj)->id;
    }
public:
    pClient find(const int id) {
        return (pClient) _find(id);
    }
};

