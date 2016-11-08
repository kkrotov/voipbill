#pragma once

#include "../classes/ObjList.h"
#include "../models/Operator.h"
#include "../classes/AppBill.h"

class OperatorList : public ObjList<Operator> {
protected:

    string sql(BDb * db) {
        return "   select  id, pricelist_id, term_in_cost, local_network_pricelist_id, client_7800_pricelist_id, operator_7800_pricelist_id " \
            "   from billing.operator " \
            "   where region = 0 or region in " + app().conf.get_sql_regions_list() +
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Operator * item) {
        item->id = row.get_i(0);
        item->pricelist_id = row.get_i(1);
        item->term_in_cost = (int)(row.get_d(2)*10000);
        item->local_network_pricelist_id = row.get_i(3);
        item->client_7800_pricelist_id = row.get_i(4);
        item->operator_7800_pricelist_id = row.get_i(5);
    }

    struct key_id {
        bool operator() (const Operator & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Operator & right) {
            return id < right.id;
        }
    };

public:
    Operator * find(int id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }
};
