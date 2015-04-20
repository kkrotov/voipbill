#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/NetworkPrefix.h"
#include "../classes/AppBill.h"

class NetworkPrefixList : public ObjList<NetworkPrefix> {
protected:

    string sql(BDb * db) {
        string time = string_date(dt);
        string server_id = app().conf.str_instance_id;
        return "	select operator_id, prefix, network_type_id, extract(epoch from date_from), extract(epoch from date_to) from billing.network_prefix " \
            "	where instance_id = '" + server_id + "' and deleted=false and date_to > now() " \
            "	order by operator_id asc, prefix asc, date_from asc";
    }

    inline void parse_item(BDbResult &row, NetworkPrefix * item) {
        item->operator_id = row.get_i(0);
        strcpy(item->prefix, row.get(1));
        item->network_type_id = row.get_i(2);
        item->date_from = row.get_ll(3);
        item->date_to = row.get_ll(4);
    }

    struct key_operator_id {
        bool operator() (const NetworkPrefix & left, int operator_id) {
            return left.operator_id < operator_id;
        }
        bool operator() (int operator_id, const NetworkPrefix & right) {
            return operator_id < right.operator_id;
        }
    };

    struct key_prefix {
        bool operator() (const NetworkPrefix & left, char * prefix) {
            return strcmp(left.prefix, prefix) < 0;
        }
        bool operator() (char * prefix, const NetworkPrefix & right) {
            return strcmp(prefix, right.prefix) < 0;
        }
    };

    struct key_timestamp {
        bool operator() (const NetworkPrefix & left, time_t timestamp) {
            return left.date_to < timestamp;
        }
        bool operator() (time_t timestamp, const NetworkPrefix & right) {
            return timestamp < right.date_from;
        }
    };

    NetworkPrefix * _find(int operator_id, char * prefix, time_t timestamp) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, operator_id, key_operator_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, prefix, key_prefix());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }

public:
    NetworkPrefix * find(int operator_id, char * prefix, time_t timestamp) {
        char tmpPrefix[20];
        strcpy(tmpPrefix, prefix);
        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;
            auto result = _find(operator_id, tmpPrefix, timestamp);
            if (result != nullptr) {
                return result;
            }

            len -= 1;
        }
        return nullptr;
    }
};
