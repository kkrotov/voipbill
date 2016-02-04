#pragma once

#include "../classes/ObjList.h"
#include "../models/StatPrefixlist.h"
#include "../classes/AppBill.h"

class StatPrefixlistList : public ObjList<StatPrefixlist> {
protected:

    string sql(BDb * db) {
        return "   select id, type_id, prefixes, sub_type, country_id, region_id, city_id, exclude_operators, operators " \
            "   from billing.stat_prefixlist " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, StatPrefixlist * item) {
        item->id = row.get_i(0);
        item->type_id = row.get_i(1);

        item->str_prefixes = string(row.get(2));
        if (item->str_prefixes.size() > 0) {
            vector<string> str_prefixes_array;
            split(str_prefixes_array, item->str_prefixes, is_any_of(","));

            item->prefixes.reserve(str_prefixes_array.size());
            for (auto prefix : str_prefixes_array) {
                item->prefixes.push_back(atoll(prefix.c_str()));
            }
        }

        item->sub_type = STAT_PREFIXLIST_SUBTYPE_ALL;
        char * subType = row.get(3);
        if (subType && subType[0]) {
            if (strcmp(subType, "fixed") == 0) {
                item->sub_type = STAT_PREFIXLIST_SUBTYPE_FIXED;
            } else if (strcmp(subType, "mobile") == 0) {
                item->sub_type = STAT_PREFIXLIST_SUBTYPE_MOBILE;
            }
        }

        item->country_id = row.get_i(4);
        item->region_id = row.get_i(5);
        item->city_id = row.get_i(6);

        item->exclude_operators = row.get_b(7);

        item->str_operators = string(row.get(8));
        if (item->str_operators.size() > 0) {
            vector<string> str_operators_array;
            split(str_operators_array, item->str_operators, is_any_of(","));

            item->operators.reserve(str_operators_array.size());
            for (auto operatorId : str_operators_array) {
                item->operators.push_back(atoi(operatorId.c_str()));
            }
        }
    }

    struct key_id {
        bool operator() (const StatPrefixlist & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const StatPrefixlist &  right) {
            return id < right.id;
        }
    };

public:
    StatPrefixlist * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        StatPrefixlist * result = begin <  end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|STAT PREFIXLIST|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|STAT PREFIXLIST|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }
};
