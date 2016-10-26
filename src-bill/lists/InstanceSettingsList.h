#pragma once

#include "../classes/ObjList.h"
#include "../models/InstanceSettings.h"
#include "../classes/AppBill.h"

class InstanceSettingsList : public ObjList<InstanceSettings> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, region_id, city_geo_id, country_id, city_id, hub_id " \
                "   from billing.instance_settings " \
                "   where id = " + server_id +
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, InstanceSettings * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->region_id, sizeof(item->region_id));
        item->city_geo_id = row.get_i(2);
        item->country_id = row.get_i(3);
        item->city_id = row.get_i(4);
        item->hub_id = row.get_i(5);
    }

    struct key_id {
        bool operator() (const InstanceSettings & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const InstanceSettings & right) {
            return id < right.id;
        }
    };

public:
    InstanceSettings * find(int id) {
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

