#pragma once


#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPackagePricelist.h"


class NNPPackagePricelistList : public ObjList<NNPPackagePricelist> {
protected:

    string sql(BDb *db) {
        return "select id,tariff_id,pricelist_id " \
            "   from nnp.package_pricelist order by tariff_id";
    }

    inline void parse_item(BDbResult &row, NNPPackagePricelist *item) {
        item->id = row.get_i(0);
        item->nnp_tariff_id = row.get_i(1);
        item->pricelist_id = row.get_i(2);
    }

    struct key_tariff_id {
        bool operator()(const NNPPackagePricelist &left, int tariff_id) {
            return left.nnp_tariff_id < tariff_id;
        }

        bool operator()(int tariff_id, const NNPPackagePricelist &right) {
            return tariff_id < right.nnp_tariff_id;
        }
    };


public:

    void findPackagePricelistIds(set<NNPPackagePricelist *> &resultNNPPackagePricelist, int tariff_id,
                                 long long int num, stringstream *trace = nullptr);


};