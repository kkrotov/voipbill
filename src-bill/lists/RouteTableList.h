#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/RouteTable.h"
#include "../classes/AppBill.h"

class RouteTableList : public ObjListByInt {
protected:
    size_t item_size() {
        return sizeof (RouteTable);
    }

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name " \
            "   from auth.route_table " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        RouteTable * item = (RouteTable *) obj;
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    inline int key(const void *obj) {
        return ((RouteTable *) obj)->id;
    }
public:
    RouteTable * find(const int id) {
        return (RouteTable *) _find(id);
    }
};
