#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ServiceNumber.h"
#include "../classes/AppBill.h"

class ServiceNumberList : public ObjListByStringPeriod<ServiceNumber> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "    select did, id, client_account_id, lines_count, extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_number " \
            "       where server_id='" + server_id + "' and expire_dt > now() " \
            "       order by did asc, activation_dt asc ";
    }

    inline void parse_item(BDbResult &row, ServiceNumber * item) {
        row.fill_cs(0, item->did, sizeof(item->did));
        item->id = row.get_i(1);
        item->client_account_id = row.get_i(2);
        item->lines_count = row.get_i(3);
        item->activation_dt = row.get_ll(4);
        item->expire_dt = row.get_ll(5);
    }

    inline char * key1(ServiceNumber *item) {
        return item->did;
    }

    inline time_t key2_from(ServiceNumber *item) {
        return item->activation_dt;
    }

    inline time_t key2_to(ServiceNumber *item) {
        return item->expire_dt;
    }

public:
    ServiceNumber * find(const char * did, time_t timestamp) {
        return (ServiceNumber *) _find(did, timestamp);
    }
};
