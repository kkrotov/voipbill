#pragma once

#include "../classes/ObjList.h"
#include "../models/Number.h"
#include "../classes/AppBill.h"

class NumberList : public ObjList<Number> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name, prefixlist_ids " \
            "   from auth.number " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Number * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
        row.fill_cs(2, item->prefixlist_ids, sizeof(item->prefixlist_ids));
    }

    struct key_id {
        bool operator() (const Number & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Number & right) {
            return id < right.id;
        }
    };

public:
    Number * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        auto result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|NUMBER|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|NUMBER|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
