#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/InstanceSettings.h"
#include "../classes/AppBill.h"

class InstanceSettingsList : public ObjListByInt {
protected:
    size_t item_size() {
        return sizeof (InstanceSettings);
    }

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, region_id, city_geo_id " \
                "   from billing.instance_settings " \
                "   where id = " + server_id +
                "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, void * obj) {
        InstanceSettings * item = (InstanceSettings *) obj;
        item->id = row.get_i(0);
        row.fill_cs(1, item->region_id, sizeof(item->region_id));
        row.fill_cs(2, item->city_geo_id, sizeof(item->city_geo_id));
    }

    inline int key(const void *obj) {
        return ( (InstanceSettings *) obj)->id;
    }
public:
    InstanceSettings * find(const int id) {
        return (InstanceSettings *) _find(id);
    }
};

