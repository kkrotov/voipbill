#pragma once

#include "../classes/ObjList.h"
#include "../models/StatDestinationPrefixlists.h"
#include "../classes/AppBill.h"

class StatDestinationPrefixlistsList : public ObjList<StatDestinationPrefixlists> {
protected:

    string sql(BDb * db) {
        return "   select destination_id, prefixlist_id " \
            "   from billing.stat_destination_prefixlists " \
            "   order by destination_id asc ";
    }

    inline void parse_item(BDbResult &row, StatDestinationPrefixlists * item) {
        item->destination_id = row.get_i(0);
        item->prefixlist_id = row.get_i(1);
    }

    struct key_destination_id {
        bool operator() (const StatDestinationPrefixlists & left, int destination_id) {
            return left.destination_id < destination_id;
        }
        bool operator() (int destination_id, const StatDestinationPrefixlists &  right) {
            return destination_id < right.destination_id;
        }
    };

public:

    void findAll(vector<int> &resultPrefixlistIds, int destination_id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, destination_id, key_destination_id());
            begin = p.first;
            end = p.second;
        }

        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|STAT DESTINATION PREFIXLISTS|BY DESTINATION ID '" << destination_id << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                resultPrefixlistIds.push_back(it->prefixlist_id);

                if (trace != nullptr) {
                    *trace << "||";
                    it->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|STAT DESTINATION PREFIXLISTS|BY DESTINATION ID '" << destination_id << "'" << "\n";
            }
        }
    }
};
