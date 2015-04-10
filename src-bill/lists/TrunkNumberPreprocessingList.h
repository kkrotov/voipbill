#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/TrunkNumberPreprocessing.h"

class TrunkNumberPreprocessingList : public ObjListByIntInt {
protected:
    size_t item_size() {
        return sizeof (TrunkNumberPreprocessing);
    }

    string sql(BDb * db) {
        return "   select trunk_id, \"order\", src, noa, length, prefix " \
            "   from auth.trunk_number_preprocessing " \
            "   order by trunk_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        TrunkNumberPreprocessing * item = (TrunkNumberPreprocessing *) obj;
        item->trunk_id = row.get_i(0);
        item->order = row.get_i(1);
        item->src = row.get_b(2);
        item->noa = row.get_i(3);
        item->length = row.get_i(4);
        row.fill_cs(5, item->prefix, sizeof(item->prefix));
    }

    inline int key1(const void *obj) {
        return ((TrunkNumberPreprocessing *) obj)->trunk_id;
    }
    inline int key2(const void *obj) {
        return ((TrunkNumberPreprocessing *) obj)->order;
    }

public:
    TrunkNumberPreprocessing * find(const int trunk_id, const int order) {
        return (TrunkNumberPreprocessing *) _find(trunk_id, order);
    }
};
