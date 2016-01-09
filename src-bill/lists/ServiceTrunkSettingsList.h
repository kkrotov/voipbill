#pragma once

#include "../classes/ObjList.h"
#include "../models/ServiceTrunkSettings.h"

class ServiceTrunkSettingsList : public ObjList<ServiceTrunkSettings> {
protected:

    string sql(BDb * db) {
        return "    select id, trunk_id, \"type\", \"order\", src_number_id, dst_number_id, pricelist_id, minimum_minutes, minimum_cost " \
            "       from billing.service_trunk_settings " \
            "       order by trunk_id asc, \"type\" asc, \"order\" asc ";
    }

    inline void parse_item(BDbResult &row, ServiceTrunkSettings * item) {
        item->id = row.get_i(0);
        item->trunk_id = row.get_i(1);
        item->type = row.get_i(2);
        item->order = row.get_i(3);
        item->src_number_id = row.get_i(4);
        item->dst_number_id = row.get_i(5);
        item->pricelist_id = row.get_i(6);
        item->minimum_minutes = row.get_i(7);
        item->minimum_cost = row.get_i(8);
    }

    struct key_trunk_id {
        bool operator() (const ServiceTrunkSettings & left, int trunk_id) {
            return left.trunk_id < trunk_id;
        }
        bool operator() (int trunk_id, const ServiceTrunkSettings & right) {
            return trunk_id < right.trunk_id;
        }
    };

    struct key_type {
        bool operator() (const ServiceTrunkSettings & left, int type) {
            return left.type < type;
        }
        bool operator() (int type, const ServiceTrunkSettings & right) {
            return type < right.type;
        }
    };

    struct key_order {
        bool operator() (const ServiceTrunkSettings & left, int order) {
            return left.order < order;
        }
        bool operator() (int order, const ServiceTrunkSettings & right) {
            return order < right.order;
        }
    };

public:
    void findAll(vector<ServiceTrunkSettings *> &resultTrunkSettings, int trunk_id, int type, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, trunk_id, key_trunk_id());
            begin = p.first;
            end = p.second;
        }
        {
            auto p = equal_range(begin, end, type, key_type());
            begin = p.first;
            end = p.second;
        }

        if (begin < end) {
            if (trace != nullptr) {
                *trace << "FOUND|SERVICE TRUNK SETTINGS|BY TRUNK_ID '" << trunk_id << "', TYPE '" << type << "'" << "\n";
            }
            for (auto it = begin; it != end; ++it) {
                ServiceTrunkSettings * trunkSettings = &*it;
                resultTrunkSettings.push_back(trunkSettings);

                if (trace != nullptr) {
                    *trace << "||";
                    trunkSettings->dump(*trace);
                    *trace << "\n";
                }
            }
        } else {
            if (trace != nullptr) {
                *trace << "NOT FOUND|SERVICE TRUNK SETTINGS|BY TRUNK_ID '" << trunk_id << "', TYPE '" << type << "'" << "\n";
            }
        }
    }
};
