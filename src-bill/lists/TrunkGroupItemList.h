#pragma once

#include "../classes/ObjList.h"
#include "../models/TrunkGroupItem.h"

class TrunkGroupItemList : public ObjList<TrunkGroupItem> {
protected:

    string sql(BDb * db) {
        return "   select trunk_group_id, trunk_id " \
            "   from auth.trunk_group_item " \
            "   order by trunk_group_id asc, trunk_id asc ";
    }

    inline void parse_item(BDbResult &row, TrunkGroupItem * item) {
        item->trunk_group_id = row.get_i(0);
        item->trunk_id = row.get_i(1);
    }

    struct key_trunk_group_id {
        bool operator() (const TrunkGroupItem & left, int trunk_group_id) {
            return left.trunk_group_id < trunk_group_id;
        }
        bool operator() (int trunk_group_id, const TrunkGroupItem & right) {
            return trunk_group_id < right.trunk_group_id;
        }
    };

public:
    void findTrunkIds(vector<int> &resultTrunkIds, int trunk_group_id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_group_id, key_trunk_group_id());
            begin = p.first;
            end = p.second;
        }

        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|TRUNK GROUP ITEM|BY TRUNK_GROUP_ID '" << trunk_group_id << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                TrunkGroupItem * rule = &*it;
                resultTrunkIds.push_back(rule->trunk_id);

                if (trace != nullptr) {
                    *trace << "||";
                    rule->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|TRUNK GROUP ITEM|BY TRUNK_GROUP_ID '" << trunk_group_id << "'" << "\n";
            }
        }
    }

};
