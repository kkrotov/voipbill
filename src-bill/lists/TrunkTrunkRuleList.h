#pragma once

#include "../classes/ObjList.h"
#include "../models/TrunkTrunkRule.h"

class TrunkTrunkRuleList : public ObjList<TrunkTrunkRule> {
protected:

    string sql(BDb * db) {
        return "   select trunk_id, \"order\", trunk_group_id, number_id_filter_a, number_id_filter_b " \
            "   from auth.trunk_trunk_rule " \
            "   order by trunk_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, TrunkTrunkRule * item) {
        item->trunk_id = row.get_i(0);
        item->order = row.get_i(1);
        item->trunk_group_id = row.get_i(2);
        item->number_id_filter_a = row.get_i(3);
        item->number_id_filter_b = row.get_i(4);
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

    void findTrunkGroupRules(vector<TrunkTrunkRule> &resultTrunkGroupRules, int trunk_id, stringstream *trace = nullptr) {
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

                resultTrunkGroupRules.push_back(TrunkTrunkRule(rule));

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
