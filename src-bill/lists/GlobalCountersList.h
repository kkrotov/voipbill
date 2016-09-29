#pragma once

#include "../classes/ObjList.h"
#include "../models/GlobalCounters.h"
#include "../classes/AppBill.h"

class GlobalCountersList : public ObjList<GlobalCounters> {
protected:
    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        string sDay = string_date(get_tday(time(nullptr)), 9);
        string sMonth = string_date(get_tmonth(time(nullptr)), 10);
        return "   select account_id, sum(sum), " \
           "          sum(case when amount_day = '" + sDay + "' then sum_day else 0 end), " \
           "          sum(case when amount_day = '" + sDay + "' then sum_mn_day else 0 end), " \
           "          sum(case when amount_month = '" + sMonth + "' then sum_month else 0 end) " \
           "   from billing.stats_account " \
           "     where server_id<>" + server_id + " and (sum<>0 or sum_day<>0 or sum_mn_day<>0 or sum_month<>0) " \
           "     group by account_id " \
           "     order by account_id asc ";
    }

    inline void parse_item(BDbResult &row, GlobalCounters * item) {
        item->client_id = row.get_i(0);
        item->sum = row.get_d(1);
        item->sum_day = row.get_d(2);
        item->sum_mn_day = row.get_d(3);
        item->sum_month = row.get_d(4);
    }

    struct key_client_id {
        bool operator() (const GlobalCounters & left, int client_id) {
            return left.client_id < client_id;
        }
        bool operator() (int client_id, const GlobalCounters & right) {
            return client_id < right.client_id;
        }
    };

public:
    GlobalCounters * find(int client_id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, client_id, key_client_id());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }
};
