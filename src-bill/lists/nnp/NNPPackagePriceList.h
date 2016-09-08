#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPackagePrice.h"
#include "../../classes/AppBill.h"

class NNPPackagePriceList : public ObjList<NNPPackagePrice> {
protected:

    string sql(BDb *db) {
        return "select id,tariff_id,destination_id,price " \
            "   from nnp.package_price ";
    }

    inline void parse_item(BDbResult &row, NNPPackagePrice *item) {
        item->id = row.get_i(0);
        item->nnp_tariff_id = row.get_i(1);
        item->nnp_destination_id = row.get_i(2);
        item->price = row.get_d(3);
    }

    struct key_id {
        bool operator()(const NNPPackagePrice &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPPackagePrice &right) {
            return id < right.id;
        }
    };

public:
    NNPPackagePrice *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPPackagePrice *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPPackagePrice|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPPackagePrice|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
