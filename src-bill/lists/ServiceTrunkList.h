#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ServiceTrunk.h"
#include "../classes/AppBill.h"

class ServiceTrunkList : public ObjListByStringPeriod<ServiceTrunk> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "    select trunk_name, id, client_account_id, " \
            "               orig_enabled, term_enabled, orig_min_payment, term_min_payment, " \
            "               extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_trunk " \
            "       where server_id='" + server_id + "' and expire_dt > now() " \
            "       order by trunk_name asc, activation_dt asc ";
    }

    inline void parse_item(BDbResult &row, ServiceTrunk * item) {
        row.fill_cs(0, item->trunk_name, sizeof(item->trunk_name));
        item->id = row.get_i(1);
        item->client_account_id = row.get_i(2);
        item->orig_enabled = row.get_b(3);
        item->term_enabled = row.get_b(4);
        item->orig_min_payment = row.get_d(5);
        item->term_min_payment = row.get_d(6);
        item->activation_dt = row.get_ll(7);
        item->expire_dt = row.get_ll(8);
    }


    inline char * key1(ServiceTrunk *item) {
        return item->trunk_name;
    }

    inline time_t key2_from(ServiceTrunk *item) {
        return item->activation_dt;
    }

    inline time_t key2_to(ServiceTrunk *item) {
        return item->expire_dt;
    }

public:
    ServiceTrunk * find(const char * trunk_name, time_t timestamp) {
        return (ServiceTrunk *) _find(trunk_name, timestamp);
    }
};
