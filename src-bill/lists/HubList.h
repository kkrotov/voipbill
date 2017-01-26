#pragma once

#include "../classes/ObjList.h"
#include "../models/Hub.h"
#include "../classes/AppBill.h"

class HubList : public ObjList<Hub> {
protected:

    string sql(BDb *db) {
        return "   select id, name " \
            "   from auth.hub " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Hub *item) {
        item->id = row.get_i(0);
        item->name = row.get_s(1);
    }

    struct key_id {
        bool operator()(const Hub &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const Hub &right) {
            return id < right.id;
        }
    };

public:
    Hub *find(int id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        return begin < end ? &*begin : nullptr;
    }

};
