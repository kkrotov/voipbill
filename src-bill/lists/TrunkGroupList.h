#pragma once

#include "../classes/ObjList.h"
#include "../models/TrunkGroup.h"
#include "../classes/AppBill.h"

class TrunkGroupList : public ObjList<TrunkGroup> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name " \
            "   from auth.trunk_group " \
            "   where server_id = " + server_id +
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, TrunkGroup * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
    }

    struct key_id {
        bool operator() (const TrunkGroup & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const TrunkGroup & right) {
            return id < right.id;
        }
    };

public:
    TrunkGroup * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        TrunkGroup * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|TRUNK GROUP|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|TRUNK GROUP|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
