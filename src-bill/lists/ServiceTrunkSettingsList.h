#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ServiceTrunkSettings.h"

class ServiceTrunkSettingsList : public ObjListByIntBoolInt<ServiceTrunkSettings> {
protected:

    string sql(BDb * db) {
        return "    select trunk_id, orig, \"order\", src_number_id, dst_number_id, pricelist_id " \
            "       from billing.service_trunk_settings " \
            "       order by trunk_id asc, orig asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, ServiceTrunkSettings * item) {
        item->trunk_id = row.get_i(0);
        item->orig = row.get_i(1);
        item->order = row.get_i(2);
        item->src_number_id = row.get_i(3);
        item->dst_number_id = row.get_i(4);
        item->pricelist_id = row.get_i(5);
    }

    inline int key1(ServiceTrunkSettings *item) {
        return item->trunk_id;
    }

    inline bool key2(ServiceTrunkSettings *item) {
        return item->orig;
    }

    inline int key3(ServiceTrunkSettings *item) {
        return item->order;
    }

public:
    ServiceTrunkSettings * find(const int trunk_id, bool orig, int order) {
        return (ServiceTrunkSettings *) _find(trunk_id, orig, order);
    }
};
