#pragma once

#include "../../classes/ObjList.h"
#include "../../models/nnp/NNPPackageMinute.h"
#include "../../classes/AppBill.h"

class NNPPackageMinuteList : public ObjList<NNPPackageMinute> {
protected:

    string sql(BDb *db) {
        return "select id,tariff_id,destination_id,minute " \
            "   from nnp.package_minute ";
    }

    inline void parse_item(BDbResult &row, NNPPackageMinute *item) {
        item->id = row.get_i(0);
        item->nnp_tariff_id = row.get_i(1);
        item->nnp_destination_id = row.get_i(2);
        item->minute = row.get_i(3);
    }

    struct key_id {
        bool operator()(const NNPPackageMinute &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const NNPPackageMinute &right) {
            return id < right.id;
        }
    };

public:
    NNPPackageMinute *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        NNPPackageMinute *result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NNPPackageMinute|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NNPPackageMinute|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
