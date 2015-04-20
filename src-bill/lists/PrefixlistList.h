#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Prefixlist.h"
#include "../classes/AppBill.h"

class PrefixlistList : public ObjList<Prefixlist> {
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

    struct key_id {
        bool operator() (const Prefixlist & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Prefixlist & right) {
            return id < right.id;
        }
    };

public:
    Prefixlist * find(int id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }
};
