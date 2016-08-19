#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPackagePricelist.h"
#include "../../classes/AppBill.h"

class NNPPackagePricelistList : public ObjList<NNPPackagePricelist> {
protected:

    string sql(BDb *db) {
        return "select id,tariff_id,pricelist_id " \
            "   from nnp.package_pricelist ";
    }

    inline void parse_item(BDbResult &row, NNPPackagePricelist *item) {
        item->id = row.get_i(0);
        item->nnp_tariff_id = row.get_i(1);
        item->pricelist_id = row.get_i(2);
    }

    struct key_id {
        bool operator()(const NNPPackagePricelist &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPPackagePricelist &right) {
            return id < right.id;
        }
    };

public:
    NNPPackagePricelist *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPPackagePricelist *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPPackagePricelist|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPPackagePricelist|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
