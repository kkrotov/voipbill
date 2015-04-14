#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Usage.h"
#include "../classes/AppBill.h"

class UsageList : public ObjListByLongPeriod<Usage> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "      select phone_num::BIGINT, id, client_id, no_of_lines, extract(epoch from actual_from), extract(epoch from actual_to) " \
            "       from billing.usage " \
            "       where region='" + server_id + "' and actual_to > now() " \
            "       order by phone_num::BIGINT asc, actual_from asc ";
    }

    inline void parse_item(BDbResult &row, Usage * item) {
        item->phone_num = row.get_ll(0);
        item->id = row.get_i(1);
        item->client_id = row.get_i(2);
        item->no_of_lines = row.get_i(3);
        item->actual_from = row.get_ll(4);
        item->actual_to = row.get_ll(5);
    }

    inline long long int key1(Usage *item) {
        return item->phone_num;
    }

    inline time_t key2_from(Usage *item) {
        return item->actual_from;
    }

    inline time_t key2_to(Usage *item) {
        return item->actual_to;
    }

public:
    Usage * find(const char * phone_num, time_t timestamp) {
        return (Usage *) _find(atoll(phone_num), timestamp);
    }

    Usage * find(const long long int phone_num, time_t timestamp) {
        return (Usage *) _find(phone_num, timestamp);
    }
};
