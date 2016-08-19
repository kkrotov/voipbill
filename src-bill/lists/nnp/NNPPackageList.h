#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPackage.h"
#include "../../classes/AppBill.h"

class NNPPackageList : public ObjList<NNPPackage> {
protected:

    string sql(BDb *db) {
        return "select tariff_id " \
            "   from nnp.package ";
    }

    inline void parse_item(BDbResult &row, NNPPackage *item) {
        item->id = row.get_i(0);
    }

    struct key_id {
        bool operator()(const NNPPackage &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPPackage &right) {
            return id < right.id;
        }
    };

public:
    NNPPackage *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPPackage *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPPackage|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPPackage|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
