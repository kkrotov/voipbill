#pragma once

#include "../classes/ObjList.h"
#include "../models/RouteTable.h"
#include "../classes/AppBill.h"

class RouteTableList : public ObjList<RouteTable> {
protected:

    string sql(BDb * db) {
        return "   select id, name " \
               "   from auth.route_table " \
               "   where server_id in " + app().conf.get_sql_regions_for_load_list_list() +
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, RouteTable * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    struct key_id {
        bool operator() (const RouteTable & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const RouteTable & right) {
            return id < right.id;
        }
    };

public:
    RouteTable * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        RouteTable * result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|ROUTE TABLE|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|ROUTE TABLE|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
