#pragma once

#include "../classes/ObjList.h"
#include "../models/TrunkRule.h"

class TrunkRuleList : public ObjList<TrunkRule> {
protected:

    string sql(BDb * db) {
        return "   select trunk_id, outgoing, \"order\", prefixlist_id " \
            "   from auth.trunk_rule " \
            "   order by trunk_id asc, outgoing asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, TrunkRule * item) {
        item->trunk_id = row.get_i(0);
        item->outgoing = row.get_b(1);
        item->order = row.get_i(2);
        item->prefixlist_id = row.get_i(3);
    }

    struct key_trunk_id {
        bool operator() (const TrunkRule & left, int trunk_id) {
            return left.trunk_id < trunk_id;
        }
        bool operator() (int trunk_id, const TrunkRule & right) {
            return trunk_id < right.trunk_id;
        }
    };

    struct key_outgoing {
        bool operator() (const TrunkRule & left, bool outgoing) {
            return left.outgoing < outgoing;
        }
        bool operator() (bool outgoing, const TrunkRule & right) {
            return outgoing < right.outgoing;
        }
    };

    struct key_order {
        bool operator() (const TrunkRule & left, int order) {
            return left.order < order;
        }
        bool operator() (int order, const TrunkRule & right) {
            return order < right.order;
        }
    };

public:
    TrunkRule * find(int trunk_id, bool outgoing, int order) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, outgoing, key_outgoing());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, order, key_order());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }


    void findAll(vector<TrunkRule *> &resultRules, int trunk_id, bool outgoing, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, outgoing, key_outgoing());
            begin = p.first;
            end = p.second;
        }

        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|TRUNK RULES|BY TRUNK_ID '" << trunk_id << "', OUTGOING '" << outgoing << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                TrunkRule * rule = &*it;
                resultRules.push_back(rule);

                if (trace != nullptr) {
                    *trace << "||";
                    rule->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|TRUNK RULES|BY TRUNK_ID '" << trunk_id << "', OUTGOING '" << outgoing << "'" << "\n";
            }
        }
    }
};
