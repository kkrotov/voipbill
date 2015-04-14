#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/TrunkRule.h"

class TrunkRuleList : public ObjListByIntBoolInt<TrunkRule> {
protected:

    string sql(BDb * db) {
        return "   select trunk_id, outgoing, \"order\", prefixlist_id " \
            "   from auth.trunk_rule " \
            "   order by trunk_id asc, outgoing asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, TrunkRule * item) {
        item->trunk_id = row.get_i(0);
        item->outgoing = row.get_b(1);
        item->order = row.get_i(2);
        item->prefixlist_id = row.get_i(3);
    }

    inline int key1(TrunkRule *item) {
        return item->trunk_id;
    }
    inline bool key2(TrunkRule *item) {
        return item->outgoing;
    }
    inline int key3(TrunkRule *item) {
        return item->order;
    }
public:
    TrunkRule * find(const int trunk_id, const bool outgoing, const int order) {
        return (TrunkRule *) _find(trunk_id, outgoing, order);
    }
};
