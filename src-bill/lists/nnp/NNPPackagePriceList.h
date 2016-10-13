#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPackagePrice.h"
#include "../../classes/AppBill.h"

class NNPPackagePriceList : public ObjList<NNPPackagePrice> {
protected:

    string sql(BDb *db) {
        return "select id,tariff_id,destination_id,price " \
            "   from nnp.package_price order by tariff_id";
    }

    inline void parse_item(BDbResult &row, NNPPackagePrice *item) {
        item->id = row.get_i(0);
        item->nnp_tariff_id = row.get_i(1);
        item->nnp_destination_id = row.get_i(2);
        item->price = row.get_d(3);
    }


    struct key_tariff_id {
        bool operator()(const NNPPackagePrice &left, int tariff_id) {
            return left.nnp_tariff_id < tariff_id;
        }

        bool operator()(int tariff_id, const NNPPackagePrice &right) {
            return tariff_id < right.nnp_tariff_id;
        }
    };

public:

    void findPackagePriceIds(set<pair<double, int>> &resultNNPPackagePriceIds, int tariff_id,
                             set<int> &nnpDestinationIds,
                             stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, tariff_id, key_tariff_id());
            begin = p.first;
            end = p.second;
        }

        if (begin < end) {
            for (auto it = begin; it != end; ++it) {
                NNPPackagePrice *package = &*it;
                if (nnpDestinationIds.count(package->nnp_destination_id) > 0) {
                    resultNNPPackagePriceIds.insert(pair<double, int>(package->price, package->id));

                    if (trace != nullptr) {
                        *trace << "FOUND|NNP PACKAGE PRICE|BY NNP_TARIFF_ID '" << tariff_id << "'" << "\n";
                        *trace << "||";
                        package->dump(*trace);
                        *trace << "\n";
                    }
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|NNP PACKAGE PRICE|BY NNP_TARIFF_ID '" << tariff_id << "'" << "\n";
            }
        }
    }
};