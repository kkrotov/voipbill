#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/NetworkPrefix.h"

class NetworkPrefixList : public ObjList<NetworkPrefix> {
protected:

    string sql(BDb * db) {
        return "	select network_config_id, prefix, network_type_id, extract(epoch from date_from), extract(epoch from date_to) from billing.network_prefix " \
            "	where deleted=false " \
            "	order by network_config_id asc, prefix asc, date_from asc";
        // and date_to > now()
    }

    inline void parse_item(BDbResult &row, NetworkPrefix * item) {
        item->network_config_id = row.get_i(0);
        strcpy(item->prefix, row.get(1));
        item->network_type_id = row.get_i(2);
        item->date_from = row.get_ll(3);
        item->date_to = row.get_ll(4);
    }

    struct key_network_config_id {
        bool operator() (const NetworkPrefix & left, int network_config_id) {
            return left.network_config_id < network_config_id;
        }
        bool operator() (int network_config_id, const NetworkPrefix & right) {
            return network_config_id < right.network_config_id;
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

    NetworkPrefix * _find(int network_config_id, char * prefix, time_t timestamp) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, network_config_id, key_network_config_id());
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
    NetworkPrefix * find(int network_config_id, long long int numberPrefix, time_t timestamp, stringstream *trace = nullptr) {

        char tmpPrefix[20];
        sprintf(tmpPrefix, "%lld", numberPrefix);

        int len = strlen(tmpPrefix);
        while (len > 0) {
            tmpPrefix[len] = 0;

            auto result = _find(network_config_id, tmpPrefix, timestamp);
            if (result != nullptr) {

                if (trace != nullptr) {
                    *trace << "FOUND|NETWORK PREFIX|BY NETWORK CONFIG ID '" << network_config_id << "', NUMBER '" << numberPrefix << "', time '" << timestamp << "'" << "\n";
                    *trace << "||";
                    result->dump(*trace);
                    *trace << "\n";
                }

                return result;
            }

            len -= 1;
        }

        if (trace != nullptr) {
            *trace << "NOT FOUND|NETWORK PREFIX|BY NETWORK CONFIG ID '" << network_config_id << "', NUMBER '" << numberPrefix << "', time '" << timestamp << "'" << "\n";
        }

        return nullptr;
    }
};
