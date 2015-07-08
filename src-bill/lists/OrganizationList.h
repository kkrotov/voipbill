#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/Organization.h"

class OrganizationList : public ObjList<Organization> {
protected:

    string sql(BDb * db) {
        return "   select id, organization_id, extract(epoch from actual_from), extract(epoch from actual_to), tax_rate " \
                "   from billing.organization " \
                "   order by id asc, actual_from asc ";
    }

    inline void parse_item(BDbResult &row, Organization * item) {
        item->id = row.get_i(0);
        item->organization_id = row.get_i(1);
        item->actual_from = row.get_ll(2);
        item->actual_to = row.get_ll(3) + 86400 - 1;
        item->vat_rate = row.get_d(4) / 100.0;
    }

    struct key_organization_id {
        bool operator() (const Organization & left, int organization_id) {
            return left.organization_id < organization_id;
        }
        bool operator() (int organization_id, const Organization & right) {
            return organization_id < right.organization_id;
        }
    };

    struct key_timestamp {
        bool operator() (const Organization & left, time_t timestamp) {
            return left.actual_to < timestamp;
        }
        bool operator() (time_t timestamp, const Organization & right) {
            return timestamp < right.actual_from;
        }
    };

public:
    Organization * find(int organization_id, time_t timestamp, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, organization_id, key_organization_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, timestamp, key_timestamp());
            begin = p.first;
            end = p.second;
        }
        auto result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {

            if (result != nullptr) {
                *trace << "FOUND|ORGANIZATION|BY ORGANIZATION_ID '" << organization_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|ORGANIZATION|BY ORGANIZATION_ID '" << organization_id << "', TIME '" << string_time(timestamp) << "'" << "\n";
            }
        }

        return result;
    }
};

