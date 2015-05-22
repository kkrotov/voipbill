#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Trunk.h"
#include "../classes/AppBill.h"

class TrunkByAliasList : public ObjList<Trunk> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name, trunk_name_alias, code, source_rule_default_allowed, destination_rule_default_allowed, default_priority, auto_routing, route_table_id, our_trunk, auth_by_number, use_redirect_number " \
            "   from auth.trunk " \
            "   where server_id = " + server_id + " and trunk_name_alias is not null "
            "   order by trunk_name_alias asc ";
    }

    inline void parse_item(BDbResult &row, Trunk * item) {
        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
        row.fill_cs(2, item->trunk_name, sizeof(item->trunk_name));
        item->code = row.get_i(3);
        item->source_rule_default_allowed = row.get_b(4);
        item->destination_rule_default_allowed = row.get_b(5);
        item->default_priority = row.get_i(6);
        item->auto_routing = row.get_b(7);
        item->route_table_id = row.get_i(8);
        item->our_trunk = row.get_b(9);
        item->auth_by_number = row.get_b(10);
        item->use_redirect_number = row.get_b(11);
    }

    struct key_trunk_name {
        bool operator() (const Trunk & left, const char * trunk_name) {
            return strcmp(left.trunk_name, trunk_name) < 0;
        }
        bool operator() (const char * trunk_name, const Trunk & right) {
            return strcmp(trunk_name, right.trunk_name) < 0;
        }
        bool operator() (const Trunk & left, const Trunk & right) {
            return strcmp(left.trunk_name, right.trunk_name) < 0;
        }
    };

public:
    Trunk * find(const char * trunk_name, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_name, key_trunk_name());
            begin = p.first;
            end = p.second;
        }
        auto result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|TRUNK|BY ALIAS '" << trunk_name << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|TRUNK|BY ALIAS '" << trunk_name << "'" << "\n";
            }
        }

        return result;
    }

    void load(BDb *db) override {
        ObjList<Trunk>::load(db);

        sort(data.begin(), data.end(), key_trunk_name());
    };
};
