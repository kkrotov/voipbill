#pragma once

#include "../classes/ObjList.h"
#include "../models/Tariff.h"
#include "../classes/AppBill.h"

class TariffList : public ObjList<Tariff> {
protected:

    string sql(BDb * db) {
        return "   select id, freemin, freemin_for_number, pricelist_id, paid_redirect, tariffication_by_minutes, tariffication_full_first_minute, tariffication_free_first_seconds " \
            "   from billing.tariff " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Tariff * item) {
        item->id = row.get_i(0);
        item->freemin = row.get_i(1);
        item->freemin_for_number = row.get_b(2);
        item->pricelist_id = row.get_i(3);
        item->paid_redirect = row.get_b(4);
        item->tariffication_by_minutes = row.get_b(5);
        item->tariffication_full_first_minute = row.get_b(6);
        item->tariffication_free_first_seconds = row.get_b(7);
    }

    struct key_id {
        bool operator() (const Tariff & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Tariff &  right) {
            return id < right.id;
        }
    };

public:
    Tariff * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        Tariff * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|TARIFF|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|TARIFF|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
