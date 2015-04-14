#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/TariffChangeLog.h"

class TariffChangeLogList : public ObjListByIntPeriod<TariffChangeLog> {
protected:

    string sql(BDb * db) {
        return "    select l.usage_id, l.tarif_id_local, l.tarif_id_local_mob, l.tarif_id_russia, l.tarif_id_russia_mob, l.tarif_id_intern, l.tarif_id_sng, " \
                "           extract(epoch from l.date_activation) " \
                "   from ( \n"
                "            select usage_id, max(id) as max_id from billing.tarif_change_log \n"
                "            group by usage_id \n"
                "   ) as lm \n"
                "   inner join billing.tarif_change_log l on l.id = lm.max_id" \
                "   order by l.usage_id asc, l.date_activation ";
    }

    inline void parse_item(BDbResult &row, TariffChangeLog * item) {
        item->usage_id = row.get_i(0);
        item->tariff_id_local = row.get_i(1);
        item->tariff_id_local_mob = row.get_i(1);
        item->tariff_id_russia = row.get_i(2);
        item->tariff_id_russia_mob = row.get_i(3);
        item->tariff_id_intern = row.get_i(4);
        item->tariff_id_sng = row.get_i(5);
        item->activation_date = row.get_ll(6);
    }

public:
    void load(BDb *db, time_t dt) override {
        ObjList::load(db, dt);

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

    inline int key1(TariffChangeLog *item) {
        return item->usage_id;
    }

    inline time_t key2_from(TariffChangeLog *item) {
        return item->activation_date;
    }

    inline time_t key2_to(TariffChangeLog *item) {
        return item->expire_dt;
    }
public:
    TariffChangeLog * find(const int usage_id, time_t timestamp) {
        return (TariffChangeLog *) _find(usage_id, timestamp);
    }
};
