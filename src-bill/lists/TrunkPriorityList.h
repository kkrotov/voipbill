#pragma once

#include "../classes/ObjList.h"
#include "../models/TrunkPriority.h"

class TrunkPriorityList : public ObjList<TrunkPriority> {
protected:

    string sql(BDb * db) {
        return "   select id, trunk_id, \"order\", priority, number_id_filter_a, number_id_filter_b, trunk_group_id " \
            "   from auth.trunk_priority " \
            "   order by trunk_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, TrunkPriority * item) {
        item->id = row.get_i(0);
        item->trunk_id = row.get_i(1);
        item->order = row.get_i(2);
        item->priority = row.get_i(3);
        item->number_id_filter_a = row.get_i(4);
        item->number_id_filter_b = row.get_i(5);
        item->trunk_group_id = row.get_i(6);
    }

    struct key_trunk_id {
        bool operator() (const TrunkPriority & left, int trunk_id) {
            return left.trunk_id < trunk_id;
        }
        bool operator() (int trunk_id, const TrunkPriority & right) {
            return trunk_id < right.trunk_id;
        }
    };

    struct key_order {
        bool operator() (const TrunkPriority & left, int order) {
            return left.order < order;
        }
        bool operator() (int order, const TrunkPriority & right) {
            return order < right.order;
        }
    };

public:
    TrunkPriority * find(int trunk_id, int order) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
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


    void findAllTrunkPriority(int trunk_id, vector<TrunkPriority> &trunkPriorityList) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
            begin = p.first;
            end = p.second;
            for (auto it = begin; it != end; ++it) {
                trunkPriorityList.push_back(*it);
            }
        }

    }
};
