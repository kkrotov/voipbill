#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPrefix.h"
#include "../../classes/AppBill.h"

class NNPPrefixList : public ObjList<NNPPrefix> {
protected:

    string sql(BDb *db) {
        return "select id, name " \
            "   from nnp.prefix " \
            "   order by name ";
    }

    inline void parse_item(BDbResult &row, NNPPrefix *item) {
        item->id = row.get_i(0);
        item->name = row.get_i(1);
    }

    struct key_id {
        bool operator()(const NNPPrefix &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPPrefix &right) {
            return id < right.id;
        }
    };

public:
    NNPPrefix *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPPrefix *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPPrefix|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPPrefix|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
