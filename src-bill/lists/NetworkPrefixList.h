#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/NetworkPrefix.h"
#include "../classes/AppBill.h"

class NetworkPrefixList : public ObjListByIntLongPeriod<NetworkPrefix> {
protected:

    string sql(BDb * db) {
        string time = string_date(dt);
        string server_id = app().conf.str_instance_id;
        return "	select operator_id, prefix, network_type_id, extract(epoch from date_from), extract(epoch from date_to) from billing.network_prefix " \
            "	where instance_id = '" + server_id + "' and deleted=false and date_to > now() " \
            "	order by operator_id, prefix, date_from";
    }

    inline void parse_item(BDbResult &row, NetworkPrefix * item) {
        item->operator_id = row.get_i(0);
        item->prefix, row.get_ll(1);
        item->network_type_id = row.get_i(2);
        item->date_from = row.get_ll(3);
        item->date_to = row.get_ll(4);
    }

    inline int key1(NetworkPrefix *item) {
        return item->operator_id;
    }

    inline long long int key2(NetworkPrefix *item) {
        return item->prefix;
    }

    inline time_t key3_from(NetworkPrefix *item) {
        return item->date_from;
    }

    inline time_t key3_to(NetworkPrefix *item) {
        return item->date_to;
    }

public:
    NetworkPrefix * find(const int operator_id, const char * prefix, time_t timestamp) {
        char tmpPrefix[20];
        strcpy(tmpPrefix, prefix);
        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            long long int nPrefix = atoll(tmpPrefix);
            void * result = _find(operator_id, nPrefix, timestamp);
            if (result != 0) {
                return (NetworkPrefix *) result;
            }

            len -= 1;
        }
        return (NetworkPrefix *) 0;
    }

    NetworkPrefix * find(const int operator_id, const long long int nPrefix, time_t timestamp) {
        char prefix[20];
        sprintf(prefix, "%lld", nPrefix);

        return find(operator_id, prefix, timestamp);
    }
};
