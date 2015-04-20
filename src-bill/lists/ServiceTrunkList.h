#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ServiceTrunk.h"
#include "../classes/AppBill.h"

class ServiceTrunkList : public ObjList<ServiceTrunk> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "    select trunk_name, id, client_account_id, " \
            "               orig_enabled, term_enabled, orig_min_payment, term_min_payment, " \
            "               extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_trunk " \
            "       where (server_id='" + server_id + "' or server_id is null) and expire_dt > now() " \
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

    struct key_trunk_name {
        bool operator() (const ServiceTrunk & left, char * trunk_name) {
            return strcmp(left.trunk_name, trunk_name) < 0;
        }
        bool operator() (char * trunk_name, const ServiceTrunk & right) {
            return strcmp(trunk_name, right.trunk_name) < 0;
        }
    };

    struct key_timestamp {
        bool operator() (const ServiceTrunk & left, time_t timestamp) {
            return left.expire_dt < timestamp;
        }
        bool operator() (time_t timestamp, const ServiceTrunk & right) {
            return timestamp < right.activation_dt;
        }
    };


public:
    ServiceTrunk * find(char * trunk_name, time_t timestamp) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_name, key_trunk_name());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }

};
