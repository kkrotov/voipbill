#pragma once

#include "../classes/ObjList.h"
#include "../models/TrunkNumberPreprocessing.h"

class TrunkNumberPreprocessingList : public ObjList<TrunkNumberPreprocessing> {
protected:

    string sql(BDb * db) {
        return "   select trunk_id, \"order\", src, noa, length, prefix " \
            "   from auth.trunk_number_preprocessing " \
            "   order by trunk_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, TrunkNumberPreprocessing * item) {
        item->trunk_id = row.get_i(0);
        item->order = row.get_i(1);
        item->src = row.get_b(2);
        item->noa = row.get_i(3);
        item->length = row.get_i(4);
        row.fill_cs(5, item->prefix, sizeof(item->prefix));
    }

    struct key_trunk_id {
        bool operator() (const TrunkNumberPreprocessing & left, int trunk_id) {
            return left.trunk_id < trunk_id;
        }
        bool operator() (int trunk_id, const TrunkNumberPreprocessing & right) {
            return trunk_id < right.trunk_id;
        }
    };

    struct key_order {
        bool operator() (const TrunkNumberPreprocessing & left, int order) {
            return left.order < order;
        }
        bool operator() (int order, const TrunkNumberPreprocessing & right) {
            return order < right.order;
        }
    };

public:
    TrunkNumberPreprocessing * find(int trunk_id, int order, stringstream *trace = nullptr) {
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
        TrunkNumberPreprocessing * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|TRUNK NUMBER PREPROCESSING|BY TRUNK_ID '" << trunk_id << "', ORDER '" << order << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|TRUNK NUMBER PREPROCESSING|BY TRUNK_ID '" << trunk_id << "', ORDER '" << order << "'" << "\n";
            }
        }

        return result;
    }
};
