#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/RouteCase.h"
#include "../classes/AppBill.h"

class RouteCaseList : public ObjListByInt<RouteCase> {
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

    inline int key(RouteCase *item) {
        return item->id;
    }
public:
    RouteCase * find(const int id) {
        return (RouteCase *) _find(id);
    }
};
