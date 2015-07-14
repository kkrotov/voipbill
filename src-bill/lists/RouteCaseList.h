#pragma once

#include "../classes/ObjList.h"
#include "../models/RouteCase.h"
#include "../classes/AppBill.h"

class RouteCaseList : public ObjList<RouteCase> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name " \
            "   from auth.route_case " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, RouteCase * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    struct key_id {
        bool operator() (const RouteCase & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const RouteCase & right) {
            return id < right.id;
        }
    };

public:
    RouteCase * find(int id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }
};
