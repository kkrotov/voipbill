#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/MobPrefix.h"

class MobPrefixList : public ObjList<MobPrefix> {
protected:

    string sql(BDb * db) {
        return "   select prefix, mob" \
                "   from geo.mob_prefix " \
                "   order by prefix asc ";
    }

    inline void parse_item(BDbResult &row, MobPrefix * item) {
        strcpy(item->prefix, row.get(0));
        item->mob = row.get_b(1);
    }

    struct key_prefix {
        bool operator() (const MobPrefix & left, char * prefix) {
            return strcmp(left.prefix, prefix) < 0;
        }
        bool operator() (char * prefix, const MobPrefix & right) {
            return strcmp(prefix, right.prefix) < 0;
        }
    };

    MobPrefix * _find(char * prefix) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, prefix, key_prefix());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }

public:
    MobPrefix * find(long long int prefix, stringstream *trace = nullptr) {
        char tmpPrefix[20];
        sprintf(tmpPrefix, "%lld", prefix);
        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            auto result = _find(tmpPrefix);
            if (result != 0) {

                if (trace != nullptr) {
                    *trace << "FOUND|MOB_PREFIX|BY '" << prefix << "'" << "\n";
                    *trace << "||";
                    result->dump(*trace);
                    *trace << "\n";
                }

                return result;
            }

            len -= 1;
        }

        if (trace != nullptr) {
            *trace << "NOT FOUND|MOB_PREFIX|BY '" << prefix << "'" << "\n";
        }

        return nullptr;
    }
};
