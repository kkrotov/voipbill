#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/InstanceSettings.h"
#include "../classes/AppBill.h"

class InstanceSettingsList : public ObjListByInt<InstanceSettings> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, region_id, city_geo_id " \
                "   from billing.instance_settings " \
                "   where id = " + server_id +
                "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, InstanceSettings * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->region_id, sizeof(item->region_id));
        item->city_geo_id = row.get_i(2);
    }

    inline int key(InstanceSettings *item) {
        return item->id;
    }
public:
    InstanceSettings * find(const int id) {
        return (InstanceSettings *) _find(id);
    }
};

