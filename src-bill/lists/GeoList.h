#pragma once

#include "../classes/ObjList.h"
#include "../models/Geo.h"
#include "../classes/AppBill.h"

class GeoList : public ObjList<Geo> {
protected:
    string sql(BDb * db) {
        string server_id = app().conf.str_instance_id;
        return "   select id, country, region, city " \
            "   from geo.geo " \
            "   order by id asc ";
    }

    inline void parse_item(BDbResult &row, Geo * item) {
        item->id = row.get_i(0);
        item->country_id = row.get_i(1);
        item->region_id = row.get_i(2);
        item->city_id = row.get_i(3);
    }

    struct key_id {
        bool operator() (const Geo & left, int id) {
            return left.id < id;
        }
        bool operator() (int id, const Geo & right) {
            return id < right.id;
        }
    };

public:
    Geo * find(int id, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, id, key_id());
            begin = p.first;
            end = p.second;
        }
        Geo * result = begin < end ? &*begin : nullptr;

        if (trace != nullptr) {
            if (result != nullptr) {
                *trace << "FOUND|GEO|BY ID '" << id << "'" << "\n";
                *trace << "||";
                result->dump(*trace);
                *trace << "\n";
            } else {
                *trace << "NOT FOUND|GEO|BY ID '" << id << "'" << "\n";
            }
        }

        return result;
    }

};
