#pragma once

#include "../classes/ObjList.h"
#include "../models/PrefixlistPrefix.h"

class PrefixlistPrefixList : public ObjList<PrefixlistPrefix> {
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

    struct key_prefixlist_id {
        bool operator() (const PrefixlistPrefix & left, int prefixlist_id) {
            return left.prefixlist_id < prefixlist_id;
        }
        bool operator() (int prefixlist_id, const PrefixlistPrefix & right) {
            return prefixlist_id < right.prefixlist_id;
        }
    };

    struct key_prefix {
        bool operator() (const PrefixlistPrefix & left, char * prefix) {
            return strcmp(left.prefix, prefix) < 0;
        }
        bool operator() (char * prefix, const PrefixlistPrefix & right) {
            return strcmp(prefix, right.prefix) < 0;
        }
    };

    PrefixlistPrefix * _find(int prefixlist_id, char * prefix) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, prefixlist_id, key_prefixlist_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, prefix, key_prefix());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }

public:
    PrefixlistPrefix * find(int prefixlist_id, char * prefix) {
        char tmpPrefix[20];
        strcpy(tmpPrefix, prefix);
        size_t len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            auto result = _find(prefixlist_id, tmpPrefix);
            if (result != nullptr) {
                return result;
            }

            len -= 1;
        }
        return nullptr;
    }
};
