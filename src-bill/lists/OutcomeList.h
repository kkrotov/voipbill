#pragma once

#include "../classes/ObjList.h"
#include "../models/Outcome.h"
#include "../classes/AppBill.h"

class OutcomeList : public ObjList<Outcome> {
protected:

    string sql(BDb * db) {
        return "   select id, name, type_id, route_case_id, release_reason_id, airp_id, calling_station_id, called_station_id, " \
            "   route_case_1_id,route_case_2_id from auth.outcome " \
            "   where server_id in " + app().conf.get_sql_regions_list() +
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Outcome * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
        item->type_id = row.get_i(2);
        item->route_case_id = row.get_i(3);
        item->release_reason_id = row.get_i(4);
        item->airp_id = row.get_i(5);
        row.fill_cs(6, item->calling_station_id, sizeof(item->calling_station_id));
        row.fill_cs(7, item->called_station_id, sizeof(item->called_station_id));
        item->route_case_1_id = row.get_i(8);
        item->route_case_2_id = row.get_i(9);
    }

    struct key_id {
        bool operator() (const Outcome & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Outcome & right) {
            return id < right.id;
        }
    };

public:
    Outcome * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        Outcome * result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|OUTCOME|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|OUTCOME|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
