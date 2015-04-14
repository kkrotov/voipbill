#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Prefixlist.h"
#include "../classes/AppBill.h"

class PrefixlistList : public ObjListByInt<Prefixlist> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name " \
            "   from auth.prefixlist " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Prefixlist * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    inline int key(Prefixlist *item) {
        return item->id;
    }
public:
    Prefixlist * find(const int id) {
        return (Prefixlist *) _find(id);
    }
};
