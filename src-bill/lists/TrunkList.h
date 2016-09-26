#pragma once

#include "../classes/ObjList.h"
#include "../models/Trunk.h"
#include "../classes/AppBill.h"

class TrunkList : public ObjList<Trunk> {
protected:

    string sql(BDb *db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, name, trunk_name, code, source_rule_default_allowed, destination_rule_default_allowed, source_trunk_rule_default_allowed, default_priority, auto_routing, route_table_id, our_trunk, auth_by_number, orig_redirect_number_7800, orig_redirect_number, term_redirect_number " \
            "   from auth.trunk " \
            "   where server_id = " + server_id +
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Trunk *item) {
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
    }

    struct key_id {
        bool operator()(const Trunk &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const Trunk &right) {
            return id < right.id;
        }
    };

public:
    Trunk *find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }

        Trunk * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|TRUNK|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|TRUNK|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }

    void findAllAutorouting(vector<Trunk *> &resultTrunks, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();

        for (auto it = begin; it != end; ++it) {
            Trunk *trunk = &*it;

            if (trunk->auto_routing) {
                if (resultTrunks.size() == 0) {
                    if (trace != nullptr) {
                        *trace << "FOUND|TRUNKS|BY AUTOROUTING" << "\n";
                    }
                }

                resultTrunks.push_back(trunk);

                if (trace != nullptr) {
                    *trace << "||";
                    trunk->dump(*trace);
                    *trace << "\n";
                }
            }
        }

        if (resultTrunks.size() == 0) {
            if (trace != nullptr) {
                *trace << "NOT FOUND|TRUNKS|BY AUTOROUTING" << "\n";
            }
        }
    }
};
