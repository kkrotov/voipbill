#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/PrefixlistPrefix.h"

class PrefixlistPrefixList : public ObjListByIntPrefix {
protected:
    size_t item_size() {
        return sizeof (PrefixlistPrefix);
    }

    string sql(BDb * db) {
        return "   select prefixlist_id, prefix " \
            "   from auth.prefixlist_prefix " \
            "   order by prefixlist_id asc, prefix asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        PrefixlistPrefix * item = (PrefixlistPrefix *) obj;
        item->prefixlist_id = row.get_i(0);
        row.fill_cs(1, item->prefix, sizeof(item->prefix));
    }

    inline int key0(const void *obj) {
        return ((PrefixlistPrefix *) obj)->prefixlist_id;
    }

    inline char * key(const void *obj) {
        return ((PrefixlistPrefix *) obj)->prefix;
    }
public:
    PrefixlistPrefix * find(const int prefixlist_id, const char * prefix) {
        return (PrefixlistPrefix *) _find(prefixlist_id, prefix);
    }
};
