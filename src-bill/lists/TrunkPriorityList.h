#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/TrunkPriority.h"

class TrunkPriorityList : public ObjListByIntInt<TrunkPriority> {
protected:

    string sql(BDb * db) {
        return "   select trunk_id, \"order\", priority, prefixlist_id " \
            "   from auth.trunk_priority " \
            "   order by trunk_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, TrunkPriority * item) {
        item->trunk_id = row.get_i(0);
        item->order = row.get_i(1);
        item->priority = row.get_i(2);
        item->prefixlist_id = row.get_i(3);
    }

    inline int key1(TrunkPriority *item) {
        return item->trunk_id;
    }
    inline int key2(TrunkPriority *item) {
        return item->order;
    }
public:
    TrunkPriority * find(const int trunk_id, const int order) {
        return (TrunkPriority *) _find(trunk_id, order);
    }
};
