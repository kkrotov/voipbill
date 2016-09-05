#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPRegion.h"
#include "../../classes/AppBill.h"

class NNPRegionList : public ObjList<NNPRegion> {
protected:

    string sql(BDb *db) {
        return "select id, name " \
            "   from nnp.region " \
            "   order by name ";
    }

    inline void parse_item(BDbResult &row, NNPRegion *item) {
        item->id = row.get_i(0);
        item->name = row.get_i(1);
    }

    struct key_id {
        bool operator()(const NNPRegion &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPRegion &right) {
            return id < right.id;
        }
    };

public:
    NNPRegion *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPRegion *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPRegion|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPRegion|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
