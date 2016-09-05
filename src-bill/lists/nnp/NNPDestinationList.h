#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPDestination.h"
#include "../../classes/AppBill.h"

class NNPDestinationList : public ObjList<NNPDestination> {
protected:

    string sql(BDb * db) {
        return "select id, name " \
            "   from nnp.destination " \
            "   order by name ";
    }

    inline void parse_item(BDbResult &row, NNPDestination * item) {
        item->id = row.get_i(0);
        item->name = row.get_i(1);
    }

    struct key_id {
        bool operator() (const NNPDestination & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const NNPDestination & right) {
            return id < right.id;
        }
    };

public:
    NNPDestination * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPDestination * result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPDestination|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPDestination|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
