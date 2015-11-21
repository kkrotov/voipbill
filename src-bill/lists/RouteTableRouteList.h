#pragma once

#include "../classes/ObjList.h"
#include "../models/RouteTableRoute.h"
#include "../classes/AppBill.h"

class RouteTableRouteList : public ObjList<RouteTableRoute> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select route_table_id, \"order\", a_number_id, b_number_id, outcome_id, outcome_route_table_id " \
            "   from auth.route_table_route " \
            "   order by route_table_id asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, RouteTableRoute * item) {
        item->route_table_id = row.get_i(0);
        item->order = row.get_i(1);
        item->a_number_id = row.get_i(2);
        item->b_number_id = row.get_i(3);
        item->outcome_id = row.get_i(4);
        item->outcome_route_table_id = row.get_i(5);
    }


    struct key_route_table_id {
        bool operator() (const RouteTableRoute & left, int route_table_id) {
            return left.route_table_id < route_table_id;
        }
        bool operator() (int route_table_id, const RouteTableRoute & right) {
            return route_table_id < right.route_table_id;
        }
    };

    struct key_order {
        bool operator() (const RouteTableRoute & left, int order) {
            return left.order < order;
        }
        bool operator() (int order, const RouteTableRoute & right) {
            return order < right.order;
        }
    };

public:
    void findAll(vector<RouteTableRoute *> &resultRoutes, int route_table_id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, route_table_id, key_route_table_id());
            begin = p.first;
            end = p.second;
        }


        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|ROUTE TABLE ROUTES|BY ROUTE_TABLE_ID '" << route_table_id << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                resultRoutes.push_back(&*it);

                if (trace != nullptr) {
                    *trace << "||";
                    it->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|ROUTE TABLE ROUTES|BY ROUTE_TABLE_ID '" << route_table_id << "'" << "\n";
            }
        }
    }
};
