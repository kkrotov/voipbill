#pragma once

#include "../classes/ObjList.h"
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

    struct key_activation_dt {
        bool operator() (const ServiceNumber & left, time_t activation_dt) {
            return left.activation_dt < activation_dt;
        }
        bool operator() (time_t activation_dt, const ServiceNumber & right) {
            return activation_dt < right.activation_dt;
        }
    };

public:
    ServiceNumber * find(long long int numberPrefix, time_t timestamp, stringstream *trace = nullptr) {

        char did[20];
        sprintf(did, "%lld", numberPrefix);

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, &did[0], key_did());
            begin = p.first;
            end = p.second;
        }
        {
            end = upper_bound(begin, end, timestamp, key_activation_dt());
        }

        ServiceNumber * result = nullptr;
        if (begin < end) {
            for (auto it = begin; it != end; ++it) {
                ServiceNumber * serviceNumber = &*it;
                if (serviceNumber->expire_dt >= timestamp) {
                    result = serviceNumber;
                    break;
                }
            }
        }

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|SERVICE NUMBER|BY DID'" << numberPrefix << "', TIME '" << string_time(timestamp) << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|SERVICE NUMBER|BY DID'" << numberPrefix << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
        }

        return result;
    }
};
