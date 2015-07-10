#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Pricelist.h"

class PricelistList : public ObjList<Pricelist> {
protected:

    string sql(BDb * db) {
        return "   select  id, region, tariffication_by_minutes, tariffication_full_first_minute, orig, local, local_network_config_id, initiate_mgmn_cost, initiate_zona_cost, price_include_vat " \
            "   from billing.pricelist " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Pricelist * item) {
        item->id = row.get_i(0);
        item->region = row.get_i(1);
        item->tariffication_by_minutes = row.get_b(2);
        item->tariffication_full_first_minute = row.get_b(3);
        item->orig = row.get_b(4);
        item->local = row.get_b(5);
        item->local_network_config_id = row.get_i(6);
        item->initiate_mgmn_cost = row.get_d(7);
        item->initiate_zona_cost = row.get_d(8);
        item->price_include_vat = row.get_b(9);
    }

    struct key_id {
        bool operator() (const Pricelist & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Pricelist & right) {
            return id < right.id;
        }
    };

public:
    Pricelist * find(int id, stringstream *trace) {
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
                *trace << "FOUND|PRICELIST|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|PRICELIST|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
