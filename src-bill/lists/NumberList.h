#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Number.h"
#include "../classes/AppBill.h"

class NumberList : public ObjListByInt {
protected:
    size_t item_size() {
        return sizeof (Number);
    }

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name, prefixlist_ids " \
            "   from auth.number " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        Number * item = (Number *) obj;
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
        row.fill_cs(2, item->prefixlist_ids, sizeof(item->prefixlist_ids));
    }

    inline int key(const void *obj) {
        return ((Number *) obj)->id;
    }
public:
    Number * find(const int id) {
        return (Number *) _find(id);
    }
};
