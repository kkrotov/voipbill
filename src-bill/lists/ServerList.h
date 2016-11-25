#pragma once

#include "../classes/ObjList.h"
#include "../models/Server.h"
#include "../classes/AppBill.h"

class ServerList : public ObjList<Server> {
protected:

    string sql(BDb * db) {
        return "   select id, low_balance_outcome_id, blocked_outcome_id, min_price_for_autorouting, our_numbers_id," \
               "   calling_station_id_for_line_without_number, service_numbers, hub_id, emergency_prefixlist_id " \
            "   from public.server " \
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Server * item) {
        item->id = row.get_i(0);
        item->low_balance_outcome_id = row.get_i(1);
        item->blocked_outcome_id = row.get_i(2);
        item->min_price_for_autorouting = row.get_i(3);
        item->our_numbers_id = row.get_i(4);
        row.fill_cs(5, item->calling_station_id_for_line_without_number, sizeof(item->calling_station_id_for_line_without_number));

        string service_numbers = row.get_s(6);
        replace_all(service_numbers, " ", "");
        if (service_numbers.size() > 0) {
            split(item->service_numbers, service_numbers, is_any_of(","));
        }
        item->hub_id = row.get_i(7);
        item->emergency_prefixlist_id = row.get_i(8);
    }

    struct key_id {
        bool operator() (const Server & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Server & right) {
            return id < right.id;
        }
    };

    virtual void after_load() {
        app().conf.set_sql_regions_list(sql_regions_list());
    };

public:
    Server * find(int id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        return begin <  end ? &*begin : nullptr;
    }

    void getServersByHubId(vector<Server> &servers, int hub_id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        for (auto i = begin; i < end; i++) {
            if (i->hub_id == hub_id) servers.push_back(*i);
        }
    }

    string sql_regions_list() {
        string sql;
        int server_id = app().conf.instance_id;
        int hub_id = app().conf.hub_id;

        if (hub_id > 0) {
            vector<Server> servers;
            getServersByHubId(servers, hub_id);
            if (servers.size() > 0) {
                sql = '(';
                for (auto i:servers) {
                    sql += to_string(i.id) + ',';
                }
                auto sz = sql.size();
                if (sz > 0 && sql[sz - 1] == ',') sql.erase(sz - 1, 1);
                sql += ')';
            } else {
                sql = "(" + std::to_string(server_id) + ")";
            }

        } else {
            sql = "(" + std::to_string(server_id) + ")";
        }
        return sql;
    }
};
