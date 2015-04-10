#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/TrunkPriority.h"

class TrunkPriorityList : public ObjListByIntInt {
protected:
    size_t item_size() {
        return sizeof (TrunkPriority);
    }

    string sql(BDb * db) {
        return "   select trunk_id, \"order\", priority, prefixlist_id " \
            "   from auth.trunk_priority " \
            "   order by trunk_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        TrunkPriority * item = (TrunkPriority *) obj;
        item->trunk_id = row.get_i(0);
        item->order = row.get_i(1);
        item->priority = row.get_i(2);
        item->prefixlist_id = row.get_i(3);
    }

    inline int key1(const void *obj) {
        return ((TrunkPriority *) obj)->trunk_id;
    }
    inline int key2(const void *obj) {
        return ((TrunkPriority *) obj)->order;
    }
public:
    TrunkPriority * find(const int trunk_id, const int order) {
        return (TrunkPriority *) _find(trunk_id, order);
    }
};
