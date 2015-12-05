#pragma once

#include "../classes/ObjList.h"
#include "../models/TrunkTrunkRule.h"

class TrunkTrunkRuleList : public ObjList<TrunkTrunkRule> {
protected:

    string sql(BDb * db) {
        return "   select trunk_id, \"order\", trunk_group_id " \
            "   from auth.trunk_trunk_rule " \
            "   order by trunk_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, TrunkTrunkRule * item) {
        item->trunk_id = row.get_i(0);
        item->order = row.get_i(1);
        item->trunk_group_id = row.get_i(2);
    }

    struct key_trunk_id {
        bool operator() (const TrunkTrunkRule & left, int trunk_id) {
            return left.trunk_id < trunk_id;
        }
        bool operator() (int trunk_id, const TrunkTrunkRule & right) {
            return trunk_id < right.trunk_id;
        }
    };

public:

    void findTrunkGroupIds(vector<int> &resultTrunkGroupIds, int trunk_id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
            begin = p.first;
            end = p.second;
        }

        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|TRUNK TRUNK RULES|BY TRUNK_ID '" << trunk_id << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                TrunkTrunkRule * rule = &*it;
                resultTrunkGroupIds.push_back(rule->trunk_group_id);

                if (trace != nullptr) {
                    *trace << "||";
                    rule->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|TRUNK TRUNK RULES|BY TRUNK_ID '" << trunk_id << "'" << "\n";
            }
        }
    }
};
