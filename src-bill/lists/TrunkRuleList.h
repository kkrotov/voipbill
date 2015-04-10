#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/TrunkRule.h"

class TrunkRuleList : public ObjListByIntBoolInt {
protected:
    size_t item_size() {
        return sizeof (TrunkRule);
    }

    string sql(BDb * db) {
        return "   select trunk_id, outgoing, \"order\", prefixlist_id " \
            "   from auth.trunk_rule " \
            "   order by trunk_id asc, outgoing asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        TrunkRule * item = (TrunkRule *) obj;
        item->trunk_id = row.get_i(0);
        item->outgoing = row.get_b(1);
        item->order = row.get_i(2);
        item->prefixlist_id = row.get_i(3);
    }

    inline int key1(const void *obj) {
        return ((TrunkRule *) obj)->trunk_id;
    }
    inline bool key2(const void *obj) {
        return ((TrunkRule *) obj)->outgoing;
    }
    inline int key3(const void *obj) {
        return ((TrunkRule *) obj)->order;
    }
public:
    TrunkRule * find(const int trunk_id, const bool outgoing, const int order) {
        return (TrunkRule *) _find(trunk_id, outgoing, order);
    }
};
