#pragma once

#include "../classes/ObjList.h"
#include "../models/ServicePackage.h"
#include "../classes/AppBill.h"

class ServiceNumberPackageList : public ObjList<ServicePackage> {
protected:

    string sql(BDb * db) {
        return "    select id, service_number_id, tariff_package_id, periodical, extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_package " \
            "       where service_number_id > 0 " \
            "       order by service_number_id asc, activation_dt asc, id ";
    }

    inline void parse_item(BDbResult &row, ServicePackage * item) {
        item->id = row.get_i(0);
        item->service_number_id = row.get_i(1);
        item->service_trunk_id = 0;
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

    struct key_activation_dt {
        bool operator() (const ServicePackage & left, time_t activation_dt) {
            return left.activation_dt < activation_dt;
        }
        bool operator() (time_t activation_dt, const ServicePackage & right) {
            return activation_dt < right.activation_dt;
        }
    };

public:
    void findAll(vector<ServicePackage *> &resultPackages, int service_number_id, time_t timestamp, stringstream *trace = nullptr);
};
