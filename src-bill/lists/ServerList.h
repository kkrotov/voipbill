#pragma once

#include "../classes/ObjList.h"
#include "../models/Server.h"
#include "../classes/AppBill.h"

class ServerList : public ObjList<Server> {
protected:

    string sql(BDb *db) {
        return "select id, low_balance_outcome_id, blocked_outcome_id, min_price_for_autorouting, our_numbers_id," \
               "   calling_station_id_for_line_without_number, service_numbers, hub_id, emergency_prefixlist_id, " \
               "   h_call_sync_delay, h_cdr_sync_delay, h_call_save_delay, h_cdr_proc_wait_count, h_call_save_wait_count, " \
               "   h_thread_error_count, h_radius_request_delay, h_event_management, h_local_events, name " \
               "from public.server " \
               "order by id asc ";
    }

    inline void parse_item(BDbResult &row, Server *item) {
        item->id = row.get_i(0);
        item->low_balance_outcome_id = row.get_i(1);
        item->blocked_outcome_id = row.get_i(2);
        item->min_price_for_autorouting = row.get_i(3);
        item->our_numbers_id = row.get_i(4);
        row.fill_cs(5, item->calling_station_id_for_line_without_number,
                    sizeof(item->calling_station_id_for_line_without_number));

        string service_numbers = row.get_s(6);
        replace_all(service_numbers, " ", "");
        if (service_numbers.size() > 0) {
            split(item->service_numbers, service_numbers, is_any_of(","));
        }
        item->hub_id = row.get_i(7);
        item->emergency_prefixlist_id = row.get_i(8);
        item->call_sync_delay = get_int_vector(row.get_s(9));
        item->cdr_sync_delay = get_int_vector(row.get_s(10));
        item->call_save_delay = get_int_vector(row.get_s(11));
        item->cdr_proc_wait_count = get_int_vector(row.get_s(12));
        item->call_save_wait_count = get_int_vector(row.get_s(13));
        item->thread_error_count = get_int_vector(row.get_s(14));
        item->radius_request_delay = get_int_vector(row.get_s(15));
        item->main_event_count = get_int_vector(row.get_s(16));
        item->local_event_count = get_int_vector(row.get_s(17));
        item->name = row.get_s(18);
    }

    struct key_id {
        bool operator()(const Server &left, int id) {
            return left.id < id;
        }

        bool operator()(int id, const Server &right) {
            return id < right.id;
        }
    };

    virtual void after_load() {
        app().conf.set_sql_regions_list(sql_regions_list());
    };

public:
    Server *find(int id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        return begin < end ? &*begin : nullptr;
    }

    void getServersByHubId(vector<Server> &servers, int hub_id) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        for (auto i = begin; i < end; i++) {
            if (i->hub_id == hub_id) servers.push_back(*i);
        }
    }

    bool isRegionOnHub(int region) {
        if (app().conf.instance_id == region) {
            return true;
        }

        if (app().conf.hub_id > 0) {
            Server *server = find(region);

            if (server != nullptr && server->hub_id == app().conf.hub_id)
                return true;
        }

        return false;

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

    vector<int> get_int_vector(string keyval) {

        vector<int> v;
        if (keyval.length() > 0) {

            std::stringstream ss(keyval);
            string tmp;
            while (ss >> tmp)
                v.push_back(std::stoi(tmp));
        }
        return v;
    }

    void getSimblingRegion(set<int> &regions, int server_id) {

        regions.clear();
        Server *server = find(server_id);

        if(server != nullptr) {
            if(server->hub_id > 0) {
                auto begin = this->data.begin();
                auto end = this->data.end();
                for (auto i = begin; i < end; i++) {
                    if (i->hub_id == server->hub_id) regions.insert(i->id);
                }

            } else
            {
                regions.insert(server_id);
            }

        }

    }
};
