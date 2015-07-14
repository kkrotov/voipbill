#pragma once

#include "../classes/ObjList.h"
#include "../models/RoutingReportData.h"

class RoutingReportDataList : public ObjList<RoutingReportData> {
protected:

    string sql(BDb * db) {
        return "   select prefix, routes " \
            "   from auth.routing_report_data " \
            "   order by prefix asc ";
    }

    inline void parse_item(BDbResult &row, RoutingReportData * item) {
        row.fill_cs(0, item->prefix, sizeof(item->prefix));
        row.fill_cs(1, item->routes, sizeof(item->routes));
    }

    struct key_prefix {
        bool operator() (const RoutingReportData & left, char * prefix) {
            return strcmp(left.prefix, prefix) < 0;
        }
        bool operator() (char * prefix, const RoutingReportData & right) {
            return strcmp(prefix, right.prefix) < 0;
        }
    };

    RoutingReportData * _find(char * prefix) {
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
    RoutingReportData * find(char * prefix) {
        char tmpPrefix[20];
        strcpy(tmpPrefix, prefix);
        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            auto result = _find(tmpPrefix);
            if (result != nullptr) {
                return result;
            }

            len -= 1;
        }
        return nullptr;
    }
};
