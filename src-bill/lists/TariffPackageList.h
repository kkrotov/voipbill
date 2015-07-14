#pragma once

#include "../classes/ObjList.h"
#include "../models/TariffPackage.h"
#include "../classes/AppBill.h"

class TariffPackageList : public ObjList<TariffPackage> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, destination_id, prepaid_minutes, pricelist_id " \
            "   from billing.tariff_package " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, TariffPackage * item) {
        item->id = row.get_i(0);
        item->destination_id = row.get_i(1);
        item->prepaid_minutes = row.get_b(2);
        item->pricelist_id = row.get_i(3);
    }

    struct key_id {
        bool operator() (const TariffPackage & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const TariffPackage &  right) {
            return id < right.id;
        }
    };

public:
    TariffPackage * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        auto result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|TARIFF PACKAGE|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|TARIFF PACKAGE|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
