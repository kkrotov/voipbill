#pragma once

#include "../classes/ObjList.h"
#include "../models/ServiceNumber.h"
#include "../classes/AppBill.h"

class ServiceNumberList : public ObjList<ServiceNumber> {
protected:

    string sql(BDb * db) {
        return "    select did, id, client_account_id, lines_count, extract(epoch from activation_dt), extract(epoch from expire_dt), tech_number, tech_number_operator_id, server_id, " \
            "       server_id in " + app().conf.get_sql_regions_for_load_list_list() + " as is_local "
            "       from billing.service_number " \
            "       order by did asc, activation_dt asc ";
        //  and expire_dt > now()
    }

    inline void parse_item(BDbResult &row, ServiceNumber * item) {
        row.fill_cs(0, item->did, sizeof(item->did));
        item->id = row.get_i(1);
        item->client_account_id = row.get_i(2);
        item->lines_count = row.get_i(3);
        item->activation_dt = row.get_ll(4);
        item->expire_dt = row.get_ll(5);
        row.fill_cs(6, item->tech_number, sizeof(item->tech_number));
        item->tech_number_operator_id = row.get_i(7);
        item->server_id = row.get_i(8);
        item->is_local = row.get_b(9);
    }

    struct key {
        char * did;
        time_t timestamp;
        bool operator() (const ServiceNumber & left, int right) {
            return strcmp(left.did, did) < 0 || left.activation_dt > timestamp || left.expire_dt < timestamp;
        }
    };

    struct key_did {
        bool operator() (const ServiceNumber & left, const char * did) {
            return strcmp(left.did, did) < 0;
        }
        bool operator() (const char * did, const ServiceNumber & right) {
            return strcmp(did, right.did) < 0;
        }
    };

    struct key_activation_dt {
        bool operator() (const ServiceNumber & left, time_t activation_dt) {
            return left.activation_dt < activation_dt;
        }
        bool operator() (time_t activation_dt, const ServiceNumber & right) {
            return activation_dt < right.activation_dt;
        }
    };

public:

    ServiceNumber *find(long long int numberPrefix, time_t timestamp, stringstream *trace = nullptr);

    void findAllByClientID(vector<ServiceNumber> &resultServiceNumber, int client_id, stringstream *trace);
};
