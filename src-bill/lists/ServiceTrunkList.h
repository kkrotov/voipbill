#pragma once

#include "../classes/ObjList.h"
#include "../models/ServiceTrunk.h"
#include "../classes/AppBill.h"

class ServiceTrunkList : public ObjList<ServiceTrunk> {
protected:

    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "    select trunk_id, id, client_account_id, " \
            "               orig_enabled, term_enabled, orig_min_payment, term_min_payment, operator_id, " \
            "               extract(epoch from activation_dt), extract(epoch from expire_dt) " \
            "       from billing.service_trunk " \
            "       where server_id='" + server_id + "' " \
            "       order by trunk_id asc, activation_dt asc ";
        //  and expire_dt > now()
    }

    inline void parse_item(BDbResult &row, ServiceTrunk * item) {
        item->trunk_id = row.get_i(0);
        item->id = row.get_i(1);
        item->client_account_id = row.get_i(2);
        item->orig_enabled = row.get_b(3);
        item->term_enabled = row.get_b(4);
        item->orig_min_payment = row.get_d(5);
        item->term_min_payment = row.get_d(6);
        item->operator_id = row.get_d(7);
        item->activation_dt = row.get_ll(8);
        item->expire_dt = row.get_ll(9);
    }


    struct key_trunk_id {
        bool operator() (const ServiceTrunk & left, int trunk_id) {
            return left.trunk_id < trunk_id;
        }
        bool operator() (int trunk_id, const ServiceTrunk & right) {
            return trunk_id < right.trunk_id;
        }
    };

    struct key_timestamp {
        bool operator() (const ServiceTrunk & left, time_t timestamp) {
            return left.expire_dt < timestamp;
        }
        bool operator() (time_t timestamp, const ServiceTrunk & right) {
            return timestamp < right.activation_dt;
        }
    };


public:
    ServiceTrunk * find(int trunk_id, time_t timestamp, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }
        ServiceTrunk * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|SERVICE TRUNK|BY TRUNK ID '" << trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|SERVICE TRUNK|BY TRUNK ID '" << trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
        }

        return result;
    }

    void findAll(vector<ServiceTrunk *> &resultTrunks, int trunk_id, time_t timestamp, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }

        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|SERVICE TRUNK|BY TRUNK ID '" << trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                resultTrunks.push_back(&*it);

                if (trace != nullptr) {
                    *trace << "||";
                    it->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|SERVICE TRUNK|BY TRUNK ID '" << trunk_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
        }
    }
};


