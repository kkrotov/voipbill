#pragma once

#include "../classes/ObjList.h"
#include "../models/Airp.h"
#include "../classes/AppBill.h"

class AirpList : public ObjList<Airp> {
protected:
    string sql(BDb * db) {
        return "   select id, name " \
            "   from auth.airp " \
            "   where server_id in " + app().conf.get_sql_regions_list() +
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Airp * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    struct key_id {
        bool operator() (const Airp & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Airp & right) {
            return id < right.id;
        }
    };

public:
    Airp * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        Airp * result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|AIRP|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|AIRP|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }

};
