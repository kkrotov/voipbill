#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Airp.h"
#include "../classes/AppBill.h"

class AirpList : public ObjListByInt<Airp> {
protected:
    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name " \
            "   from auth.airp " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Airp * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    inline int key(Airp *item) {
        return item->id;
    }
public:
    Airp * find(const int id) {
        return (Airp *) _find(id);
    }
};
