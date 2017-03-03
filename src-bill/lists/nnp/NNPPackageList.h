#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPackage.h"
#include "../../classes/AppBill.h"

class NNPPackageList : public ObjList<NNPPackage> {
protected:

    string sql(BDb *db) {
        return " select tariff_id, " \
               "   tarification_free_seconds, tarification_interval_seconds, tarification_type, tarification_min_paid_seconds, "
               "   service_type_id, currency_id, is_include_vat " \
               " from nnp.package order by tariff_id";
    }

    inline void parse_item(BDbResult &row, NNPPackage *item) {
        item->id = row.get_i(0);
        item->tarification_free_seconds = row.get_i(1);
        item->tarification_interval_seconds = row.get_i(2);
        item->tarification_type = row.get_i(3);
        item->tarification_min_paid_seconds = row.get_i(4);
        item->service_type_id = row.get_i(5);
        row.fill_cs(6, item->currency_id, sizeof(item->currency_id));
        item->is_include_vat = row.get_b(7);
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
