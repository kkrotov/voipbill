#pragma once

#include "../classes/ObjList.h"
#include "../models/Trunk.h"
#include "../classes/AppBill.h"
#include "ServerList.h"

class TrunkByNameList : public ObjList<Trunk> {
protected:

    string sql(BDb * db) {
        return "   select id, name, trunk_name, code, source_rule_default_allowed, destination_rule_default_allowed, source_trunk_rule_default_allowed, default_priority, auto_routing, route_table_id, "\
               "   our_trunk, auth_by_number, orig_redirect_number_7800, orig_redirect_number, term_redirect_number, capacity, sw_minimalki,server_id,sw_shared,  " \
               "   load_warning, road_to_region, tech_trunk " \
               "   from auth.trunk " \
               "   where server_id in " + app().conf.get_sql_regions_for_load_list_list() +
               "   order by trunk_name asc ";
    }

    inline void parse_item(BDbResult &row, Trunk * item) {

        item->id = row.get_i(0);
        row.fill_cs(1, item->name, sizeof(item->name));
        row.fill_cs(2, item->trunk_name, sizeof(item->trunk_name));
        item->code = row.get_i(3);
        item->source_rule_default_allowed = row.get_b(4);
        item->destination_rule_default_allowed = row.get_b(5);
        item->source_trunk_rule_default_allowed = row.get_b(6);
        item->default_priority = row.get_i(7);
        item->auto_routing = row.get_b(8);
        item->route_table_id = row.get_i(9);
        item->our_trunk = row.get_b(10);
        item->auth_by_number = row.get_b(11);
        item->orig_redirect_number_7800 = row.get_b(12);
        item->orig_redirect_number = row.get_b(13);
        item->term_redirect_number = row.get_b(14);
        item->capacity = row.get_i(15);
        item->sw_minimalki = row.get_b(16);
        item->server_id = row.get_i(17);
        item->sw_shared = row.get_b(18);
        item->load_warning = row.get_i(19);
        item->road_to_region = row.get_i(20);
        item->tech_trunk = row.get_b(21);

    }

    struct key_trunk_name {
        bool operator() (const Trunk & left, const char * trunk_name) {
            if (!trunk_name || !left.trunk_name) {
                return false;
            }
            return strcmp(left.trunk_name, trunk_name) < 0;
        }
        bool operator() (const char * trunk_name, const Trunk & right) {
            if (!trunk_name || !right.trunk_name) {
                return false;
            }
            return strcmp(trunk_name, right.trunk_name) < 0;
        }
        bool operator() (const Trunk & left, const Trunk & right) {
            if (!right.trunk_name || !left.trunk_name) {
                return false;
            }
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
        Trunk * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|TRUNK|BY NAME '" << trunk_name << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|TRUNK|BY NAME '" << trunk_name << "'" << "\n";
            }
        }

        return result;
    }

    void load(BDb *db) override {
        ObjList<Trunk>::load(db);

        sort(data.begin(), data.end(), key_trunk_name());
    };

};
