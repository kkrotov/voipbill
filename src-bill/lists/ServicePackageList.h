#pragma once

#include "../classes/ObjList.h"
#include "../models/ServicePackage.h"
#include "../classes/AppBill.h"

class ServicePackageList : public ObjList<ServicePackage> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "    select id, service_number_id, tariff_package_id, periodical, extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_number_package " \
            "       order by service_number_id asc, activation_dt asc ";
        //  and expire_dt > now()
    }

    inline void parse_item(BDbResult &row, ServicePackage * item) {
        item->id = row.get_i(0);
        item->service_number_id = row.get_i(1);
        item->tariff_package_id = row.get_i(2);
        item->periodical = row.get_b(3);
        item->activation_dt = row.get_ll(4);
        item->expire_dt = row.get_ll(5);
    }

    struct key_service_number_id {
        bool operator() (const ServicePackage & left, const int service_number_id) {
            return left.service_number_id < service_number_id;
        }
        bool operator() (const int service_number_id, const ServicePackage & right) {
            return service_number_id < right.service_number_id;
        }
    };

    struct key_timestamp {
        bool operator() (const ServicePackage & left, time_t timestamp) {
            return left.expire_dt < timestamp;
        }
        bool operator() (time_t timestamp, const ServicePackage & right) {
            return timestamp < right.activation_dt;
        }
    };

public:
    void findAll(vector<ServicePackage *> &resultPackages, int service_number_id, time_t timestamp, stringstream *trace = nullptr) {

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, service_number_id, key_service_number_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }
        ServicePackage * result = begin <  end ? &*begin : nullptr;

        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|SERVICE NUMBER PACKAGE|BY SERVICE_NUMBER_ID '" << service_number_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                resultPackages.push_back(&*it);

                if (trace != nullptr) {
                    *trace << "||";
                    it->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|SERVICE NUMBER PACKAGE|BY SERVICE_NUMBER_ID'" << service_number_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
        }
    }
};
