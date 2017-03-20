#pragma once

#include "../classes/ObjList.h"
#include "../models/Prefixlist.h"
#include "../classes/AppBill.h"

class PrefixlistList : public ObjList<Prefixlist> {
protected:

    string sql(BDb * db) {
        return "   select id, name "
            "   from auth.prefixlist " \
            "   where server_id in " + app().conf.get_sql_regions_for_load_list_list() +
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Prefixlist * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    struct key_id {
        bool operator() (const Prefixlist & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Prefixlist & right) {
            return id < right.id;
        }
    };

public:
    Prefixlist * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        Prefixlist * result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|PREFIXLIST|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|PREFIXLIST|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
