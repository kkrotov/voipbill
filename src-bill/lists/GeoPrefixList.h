#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/GeoPrefix.h"

class GeoPrefixList : public ObjList<GeoPrefix> {
protected:

    string sql(BDb * db) {
        return "   select prefix, dest, geo_id, operator_id, region, mob" \
                "   from geo.prefix " \
                "   order by prefix asc ";
    }

    inline void parse_item(BDbResult &row, GeoPrefix * item) {
        strcpy(item->prefix, row.get(0));
        item->dest = row.get_i(1);
        item->geo_id = row.get_i(2);
        item->geo_operator_id = row.get_i(3);
        item->region_id = row.get_i(4);
        item->mob = row.get_b(5);
    }

    struct key_prefix {
        bool operator() (const GeoPrefix & left, char * prefix) {
            return strcmp(left.prefix, prefix) < 0;
        }
        bool operator() (char * prefix, const GeoPrefix & right) {
            return strcmp(prefix, right.prefix) < 0;
        }
    };

    GeoPrefix * _find(char * prefix) {
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
    GeoPrefix * find(long long int prefix, stringstream *trace = nullptr) {
        char tmpPrefix[20];
        sprintf(tmpPrefix, "%lld", prefix);
        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            auto result = _find(tmpPrefix);
            if (result != 0) {

                if (trace != nullptr) {
                    *trace << "FOUND|GEO_PREFIX|BY '" << prefix << "'" << endl;
                    *trace << "||";
                    result->dump(*trace);
                    *trace << endl;
                }

                return result;
            }

            len -= 1;
        }

        if (trace != nullptr) {
            *trace << "NOT FOUND|GEO_PREFIX|BY '" << prefix << "'" << endl;
        }

        return nullptr;
    }
};
