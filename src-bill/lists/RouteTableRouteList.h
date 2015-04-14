#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/RouteTableRoute.h"

class RouteTableRouteList : public ObjListByIntInt<RouteTableRoute> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select route_table_id, \"order\", a_number_id, b_number_id, outcome_id, outcome_route_table_id " \
            "   from auth.route_table_route " \
            "   order by route_table_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, RouteTableRoute * item) {
        item->route_table_id = row.get_i(0);
        item->order = row.get_i(1);
        item->a_number_id = row.get_i(2);
        item->b_number_id = row.get_i(3);
        item->outcome_id = row.get_i(4);
        item->outcome_route_table_id = row.get_i(5);
    }

    inline int key1(RouteTableRoute *item) {
        return item->route_table_id;
    }
    inline int key2(RouteTableRoute *item) {
        return item->route_table_id;
    }
public:
    RouteTableRoute * find(const int route_table_id, const int order) {
        return (RouteTableRoute *) _find(route_table_id, order);
    }
};
