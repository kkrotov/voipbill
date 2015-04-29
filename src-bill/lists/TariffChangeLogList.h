#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/TariffChangeLog.h"

class TariffChangeLogList : public ObjList<TariffChangeLog> {
protected:

    string sql(BDb * db) {
        return "    select l.usage_id, l.tarif_id_local, l.tarif_id_local_mob, l.tarif_id_russia, l.tarif_id_russia_mob, l.tarif_id_intern, l.tarif_id_sng, " \
                "           extract(epoch from l.date_activation) " \
                "   from ( \n"
                "            select usage_id, max(id) as max_id from billing.tariff_change_log \n"
                "            group by usage_id \n"
                "   ) as lm \n"
                "   inner join billing.tariff_change_log l on l.id = lm.max_id" \
                "   order by l.usage_id asc, l.date_activation ";
    }

    inline void parse_item(BDbResult &row, TariffChangeLog * item) {
        item->usage_id = row.get_i(0);
        item->tariff_id_local = row.get_i(1);
        item->tariff_id_local_mob = row.get_i(2);
        item->tariff_id_russia = row.get_i(3);
        item->tariff_id_russia_mob = row.get_i(4);
        item->tariff_id_intern = row.get_i(5);
        item->tariff_id_sng = row.get_i(6);
        item->activation_date = row.get_ll(7);
    }

public:
    void load(BDb *db) override {
        ObjList::load(db);

        time_t max_ts = 64060588800; // 4000-01-01

        int i = 0;
        TariffChangeLog * prevItem = nullptr;
        while (i < size()) {
            TariffChangeLog * item = get(i);

            item->expire_dt = max_ts;

            if (prevItem != nullptr && prevItem->usage_id == item->usage_id) {
                prevItem->expire_dt = item->activation_date - 1;
            }

            prevItem = item;

            i++;
        }
    }

    struct key_usage_id {
        bool operator() (const TariffChangeLog & left, int usage_id) {
            return left.usage_id < usage_id;
        }
        bool operator() (int usage_id, const TariffChangeLog & right) {
            return usage_id < right.usage_id;
        }
    };

    struct key_timestamp {
        bool operator() (const TariffChangeLog & left, time_t timestamp) {
            return left.expire_dt < timestamp;
        }
        bool operator() (time_t timestamp, const TariffChangeLog & right) {
            return timestamp < right.activation_date;
        }
    };

public:
    TariffChangeLog * find(int usage_id, time_t timestamp, stringstream *trace = nullptr) {
        auto begin = this->data.begin();
        auto end = this->data.end();
        {
            auto p = equal_range(begin, end, usage_id, key_usage_id());
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
                *trace << "FOUND|TARIFF CHANGE LOG|BY SERVICE NUMBER ID '" << usage_id << "', TIME '" << timestamp << "'" << endl;
                *trace << "||";
                result->dump(*trace);
                *trace << endl;
            } else {
                *trace << "NOT FOUND|TARIFF CHANGE LOG|BY SERVICE NUMBER ID '" << usage_id << "', TIME '" << timestamp << "'" << endl;
            }
        }

        return result;
    }
};
