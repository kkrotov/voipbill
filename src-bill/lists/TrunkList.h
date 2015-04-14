#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Trunk.h"
#include "../classes/AppBill.h"

class TrunkList : public ObjListByString<Trunk> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name, trunk_name, code, source_rule_default_allowed, destination_rule_default_allowed, default_priority, auto_routing, route_table_id, our_trunk " \
            "   from auth.trunk " \
            "   where server_id = " + server_id +
            "   order by trunk_name asc ";
    }

    inline void parse_item(BDbResult &row, Trunk * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
        row.fill_cs(2, item->trunk_name, sizeof(item->trunk_name));
        item->code = row.get_i(3);
        item->source_rule_default_allowed = row.get_b(4);
        item->destination_rule_default_allowed = row.get_b(5);
        item->default_priority = row.get_i(6);
        item->auto_routing = row.get_b(7);
        item->route_table_id = row.get_i(8);
        item->our_trunk = row.get_b(9);
    }

    inline char * key(Trunk *item) {
        return item->trunk_name;
    }
public:
    Trunk * find(const char * trunk_name) {
        return (Trunk *) _find(trunk_name);
    }
};
