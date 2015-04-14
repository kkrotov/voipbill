#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/PrefixlistPrefix.h"

class PrefixlistPrefixList : public ObjListByIntPrefix<PrefixlistPrefix> {
protected:

    string sql(BDb * db) {
        return "   select prefixlist_id, prefix " \
            "   from auth.prefixlist_prefix " \
            "   order by prefixlist_id asc, prefix asc ";
    }

    inline void parse_item(BDbResult &row, PrefixlistPrefix * item) {
        item->prefixlist_id = row.get_i(0);
        row.fill_cs(1, item->prefix, sizeof(item->prefix));
    }

    inline int key0(PrefixlistPrefix *item) {
        return item->prefixlist_id;
    }

    inline char * key(PrefixlistPrefix *item) {
        return item->prefix;
    }
public:
    PrefixlistPrefix * find(const int prefixlist_id, const char * prefix) {
        return (PrefixlistPrefix *) _find(prefixlist_id, prefix);
    }
};
