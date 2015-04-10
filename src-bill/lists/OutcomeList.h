#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Outcome.h"
#include "../classes/AppBill.h"

class OutcomeList : public ObjListByInt {
protected:
    size_t item_size() {
        return sizeof (Outcome);
    }

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name, type_id, route_case_id, release_reason_id, airp_id, calling_station_id, called_station_id " \
            "   from auth.outcome " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        Outcome * item = (Outcome *) obj;
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
        item->type_id = row.get_i(2);
        item->route_case_id = row.get_i(3);
        item->release_reason_id = row.get_i(4);
        item->airp_id = row.get_i(5);
        row.fill_cs(6, item->calling_station_id, sizeof(item->calling_station_id));
        row.fill_cs(7, item->called_station_id, sizeof(item->called_station_id));
    }

    inline int key(const void *obj) {
        return ((Outcome *) obj)->id;
    }
public:
    Outcome * find(const int id) {
        return (Outcome *) _find(id);
    }
};
