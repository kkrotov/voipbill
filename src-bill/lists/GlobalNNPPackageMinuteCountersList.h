#pragma once

#include "../classes/ObjList.h"
#include "../models/GlobalNNPPackageMinuteCounters.h"
#include "../classes/AppBill.h"

class GlobalNNPPackageMinuteCountersList : public ObjList<GlobalNNPPackageMinuteCounters> {
protected:
    string sql(BDb *db) {
        string server_id = app().conf.str_instance_id;

        return "   select nnp_account_tariff_light_id,nnp_package_minute_id, sum(used_seconds) " \
           "   from billing.stats_nnp_package_minute " \
           "     where server_id<>" + server_id + " " \
           "     group by nnp_account_tariff_light_id,nnp_package_minute_id " \
           "     order by nnp_account_tariff_light_id ";
    }

    inline void parse_item(BDbResult &row, GlobalNNPPackageMinuteCounters *item) {
        item->nnp_account_tariff_light_id = row.get_i(0);
        item->nnp_package_minute_id = row.get_i(1);
        item->used_seconds = row.get_i(2);
    }

    struct key_client_id {
        bool operator()(const GlobalNNPPackageMinuteCounters &left, int client_id) {
            return left.nnp_account_tariff_light_id < client_id;
        }

        bool operator()(int client_id, const GlobalNNPPackageMinuteCounters &right) {
            return client_id < right.nnp_account_tariff_light_id;
        }
    };

public:
    GlobalNNPPackageMinuteCounters *find(int client_id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, client_id, key_client_id());
            begin = p.first;
            end = p.second;
        }
        return begin < end ? &*begin : nullptr;
    }
};
