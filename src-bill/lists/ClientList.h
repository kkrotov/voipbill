#pragma once

#include "../classes/ObjList.h"
#include "../models/Client.h"

class ClientList : public ObjList<Client> {
protected:

    string sql(BDb * db) {
        return "   select id, voip_limit_month, voip_limit_day, credit, balance, amount_date, last_payed_month, voip_disabled, organization_id, price_include_vat, timezone_offset " \
                "   from billing.clients " \
                "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Client * item) {
        item->id = row.get_i(0);
        item->limit_m = row.get_i(1);
        item->limit_d = row.get_i(2);
        item->credit = row.get_i(3);
        item->balance = row.get_d(4);
        item->amount_date = parseDateTime(row.get(5));
        item->last_payed_month = parseDate(row.get(6));
        item->disabled = row.get_b(7);
        item->organization_id = row.get_i(8);
        item->price_include_vat = row.get_b(9);
        item->timezone_offset = row.get_i(10);
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
        auto result = begin <  end ? &*begin : nullptr;

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

