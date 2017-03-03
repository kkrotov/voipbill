#pragma once

#include "../classes/ObjList.h"
#include "../models/Client.h"

class ClientList : public ObjList<Client> {
protected:

    string sql(BDb * db) {
        return "   select id, voip_limit_day,voip_limit_mn_day, credit, balance, amount_date, voip_disabled, organization_id, price_include_vat, timezone_offset, is_blocked, anti_fraud_disabled, account_version, " \
                       " id in (select client_account_id from billing.service_trunk where now() < expire_dt and activation_dt < now()) as is_trunk_client, "
                       " id in (select client_account_id from billing.service_number where now() < expire_dt and activation_dt < now()) as is_num_client, "
                       " effective_vat_rate "
               "   from billing.clients " \
               "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Client * item) {
        item->id = row.get_i(0);
        item->limit_d = row.get_i(1);
        item->limit_d_mn = row.get_i(2);
        item->credit = row.get_i(3);
        item->balance = row.get_d(4);
        item->amount_date = parseDateTime(row.get(5));
        item->disabled = row.get_b(6);
        item->organization_id = row.get_i(7);
        item->price_include_vat = row.get_b(8);
        item->timezone_offset = (short) row.get_i(9);
        item->is_blocked = row.get_b(10);
        item->anti_fraud_disabled = row.get_b(11);
        item->account_version = row.get_i(12);
        item->is_trunk_client = row.get_b(13);
        item->is_num_client = row.get_b(14);
        item->effective_vat_rate = row.get_i(15);
    }

    struct key_id {
        bool operator() (const Client & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Client & right) {
            return id < right.id;
        }
    };

public:
    Client * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        Client * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|ACCOUNT|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|ACCOUNT|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};

