#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/ServiceNumber.h"
#include "../classes/AppBill.h"

class ServiceNumberList : public ObjList<ServiceNumber> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "    select did, id, client_account_id, lines_count, extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_number " \
            "       where server_id='" + server_id + "' " \
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

    struct key_timestamp {
        bool operator() (const ServiceNumber & left, time_t timestamp) {
            return left.expire_dt < timestamp;
        }
        bool operator() (time_t timestamp, const ServiceNumber & right) {
            return timestamp < right.activation_dt;
        }
    };

public:
    ServiceNumber * find(long long int numberPrefix, time_t timestamp, stringstream *trace = nullptr) {

        char did[20];
        sprintf(did, "%lld", numberPrefix);

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, did, key_did());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }
        auto result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|SERVICE NUMBER|BY DID'" << numberPrefix << "', TIME '" << timestamp << "'" << endl;
                *trace << "||";
                result->dump(*trace);
                *trace << endl;
            } else {
                *trace << "NOT FOUND|SERVICE NUMBER|BY DID'" << numberPrefix << "', TIME '" << timestamp << "'" << endl;
            }
        }

        return result;
    }
};