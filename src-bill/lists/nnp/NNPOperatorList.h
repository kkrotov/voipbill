#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPOperator.h"
#include "../../classes/AppBill.h"

class NNPOperatorList : public ObjList<NNPOperator> {
protected:

    string sql(BDb *db) {
        return "select id, name " \
            "   from nnp.operator " \
            "   order by name ";
    }

    inline void parse_item(BDbResult &row, NNPOperator *item) {
        item->id = row.get_i(0);
        item->name = row.get_i(1);
    }

    struct key_id {
        bool operator()(const NNPOperator &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPOperator &right) {
            return id < right.id;
        }
    };

public:
    NNPOperator *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPOperator *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPOperator|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPOperator|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
