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

        string str_prefixes(row.get(2));
        if (str_prefixes.size() > 2) {
            str_prefixes = str_prefixes.substr(1, str_prefixes.size() - 2);

            vector<string> str_prefixes_array;
            split(str_prefixes_array, str_prefixes, is_any_of(","));

            item->prefixes.reserve(str_prefixes_array.size());
            for (auto prefix : str_prefixes_array) {
                item->prefixes.push_back(atoll(prefix.c_str()));
            }
        }

        char * subType = row.get(3);
        if (strcmp(subType, "fixed") == 0) {
            item->sub_type = STAT_PREFIXLIST_SUBTYPE_FIXED;
        } else if (strcmp(subType, "mobile") == 0) {
            item->sub_type = STAT_PREFIXLIST_SUBTYPE_MOBILE;
        } else {
            item->sub_type = STAT_PREFIXLIST_SUBTYPE_ALL;
        }

        item->country_id = row.get_i(4);
        item->region_id = row.get_i(5);
        item->city_id = row.get_i(6);

        item->exclude_operators = row.get_b(7);

        string str_operators(row.get(8));
        if (str_operators.size() > 2) {
            str_operators = str_operators.substr(1, str_operators.size() - 2);

            vector<string> str_operators_array;
            split(str_operators_array, str_operators, is_any_of(","));

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
        auto result = begin <  end ? &*begin : nullptr;

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
