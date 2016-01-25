#pragma once

#include "../classes/ObjList.h"
#include "../models/ServiceTrunk.h"
#include "../classes/AppBill.h"

class ServiceTrunkList : public ObjList<ServiceTrunk> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "    select trunk_id, id, client_account_id, " \
            "               orig_enabled, term_enabled, orig_min_payment, term_min_payment, " \
            "               extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_trunk " \
            "       where server_id='" + server_id + "' " \
            "       order by trunk_id asc, activation_dt asc ";
    }

    inline void parse_item(BDbResult &row, ServiceTrunk * item) {
        item->trunk_id = row.get_i(0);
        item->id = row.get_i(1);
        item->client_account_id = row.get_i(2);
        item->orig_enabled = row.get_b(3);
        item->term_enabled = row.get_b(4);
        item->orig_min_payment = row.get_d(5);
        item->term_min_payment = row.get_d(6);
        item->activation_dt = row.get_ll(7);
        item->expire_dt = row.get_ll(8);
    }


    struct key_trunk_id {
        bool operator() (const ServiceTrunk & left, int trunk_id) {
            return left.trunk_id < trunk_id;
        }
        bool operator() (int trunk_id, const ServiceTrunk & right) {
            return trunk_id < right.trunk_id;
        }
    };

    struct key_activation_dt {
        bool operator() (const ServiceTrunk & left, time_t activation_dt) {
            return left.activation_dt < activation_dt;
        }
        bool operator() (time_t activation_dt, const ServiceTrunk & right) {
            return activation_dt < right.activation_dt;
        }
    };

public:
    ServiceTrunk * find(int trunk_id, time_t timestamp, stringstream *trace = nullptr);

    void findAll(vector<ServiceTrunk *> &resultTrunks, int trunk_id, time_t timestamp, stringstream *trace = nullptr);
};


