#pragma once

#include "../classes/ObjList.h"
#include "../models/GlobalNNPPackageMinuteCounters.h"
#include "../classes/AppBill.h"

class GlobalNNPPackageMinuteCountersList : public ObjList<GlobalNNPPackageMinuteCounters> {
protected:
    string sql(BDb *db) {

        return "   select nnp_account_tariff_light_id,nnp_package_minute_id, sum(used_seconds) " \
           "   from billing.stats_nnp_package_minute " \
           "     where server_id not in " + app().conf.get_sql_regions_list() + " " \
           "     group by nnp_account_tariff_light_id,nnp_package_minute_id " \
           "     order by nnp_account_tariff_light_id,nnp_package_minute_id ";
    }

    inline void parse_item(BDbResult &row, GlobalNNPPackageMinuteCounters *item) {
        item->nnp_account_tariff_light_id = row.get_i(0);
        item->nnp_package_minute_id = row.get_i(1);
        item->used_seconds = row.get_i(2);
    }

    struct key_account_tariff_light_id {
        bool operator()(const GlobalNNPPackageMinuteCounters &left, int client_id) {
            return left.nnp_account_tariff_light_id < client_id;
        }

        bool operator()(int client_id, const GlobalNNPPackageMinuteCounters &right) {
            return client_id < right.nnp_account_tariff_light_id;
        }
    };

    struct key_package_minute_id {
        bool operator()(const GlobalNNPPackageMinuteCounters &left, int client_id) {
            return left.nnp_package_minute_id < client_id;
        }

        bool operator()(int client_id, const GlobalNNPPackageMinuteCounters &right) {
            return client_id < right.nnp_package_minute_id;
        }
    };


public:
    int getGlobalCounter(int account_tariff_light_id, int package_minute_id) {

        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, account_tariff_light_id, key_account_tariff_light_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, package_minute_id, key_package_minute_id());
            begin = p.first;
            end = p.second;
        }

        return begin < end ? begin->used_seconds : 0;

    }
};
