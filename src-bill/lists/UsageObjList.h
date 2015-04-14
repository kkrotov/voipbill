#pragma once

#include "../../src/lists/ObjList.h"
#include "../models/UsageObj.h"
#include <stdlib.h>
#include "../classes/AppBill.h"

class UsageObjList : public ObjListBy2Long<UsageObj> {
protected:

    string sql(BDb * db) {
        string time = string_date(dt);
        string instance_id = app().conf.str_instance_id;
        return "      select  u.phone_num, u.id, u.client_id, " \
            "           t1.freemin*60* case t1.freemin_for_number when true then 1 else u.no_of_lines end, " \
            "           t1.paid_redirect, " \
            "           t1.tariffication_by_minutes, t1.tariffication_full_first_minute, t1.tariffication_free_first_seconds, " \
            "           t1.pricelist_id, t2.pricelist_id, t3.pricelist_id, t4.pricelist_id, t5.pricelist_id, t6.pricelist_id " \
            "       from billing.usage u " \
            "       left join ( " \
            "               select l.usage_id, l.tarif_id_local, l.tarif_id_local_mob, l.tarif_id_russia, l.tarif_id_russia_mob, l.tarif_id_intern, l.tarif_id_sng from ( " \
            "                       select usage_id, max(id) as max_id from billing.tarif_change_log " \
            "                       where date_activation<='" + time + "' " \
            "                       group by usage_id ) as lm " \
            "                       inner join billing.tarif_change_log l on l.id = lm.max_id " \
            "               )as lt on u.id=lt.usage_id " \
            "       left join billing.tarif t1 on t1.id = lt.tarif_id_local " \
            "       left join billing.tarif t2 on t2.id = lt.tarif_id_local_mob " \
            "       left join billing.tarif t3 on t3.id = lt.tarif_id_russia " \
            "       left join billing.tarif t4 on t4.id = lt.tarif_id_russia_mob " \
            "       left join billing.tarif t5 on t5.id = lt.tarif_id_intern " \
            "       left join billing.tarif t6 on t6.id = lt.tarif_id_sng " \
            "       where u.region='" + instance_id + "' and u.actual_from <= '" + time + "' and u.actual_to >= '" + time + "' " \
            "           and t1.pricelist_id is not null and t2.pricelist_id is not null and t3.pricelist_id is not null and t4.pricelist_id is not null and t5.pricelist_id is not null and t6.pricelist_id is not null " \
            "       order by phone_num::BIGINT asc, u.id asc ";
    }

    inline void parse_item(BDbResult &row, UsageObj * item) {
        item->phone_num = row.get_ll(0);
        item->id = row.get_i(1);
        item->client_id = row.get_i(2);
        item->free_seconds = row.get_i(3);
        item->paid_redirect = row.get_b(4);
        item->tariffication_by_minutes = row.get_b(5);
        item->tariffication_full_first_minute = row.get_b(6);
        item->tariffication_free_first_seconds = row.get_b(7);
        item->pl_local_id = row.get_i(8);
        item->pl_local_mob_id = row.get_i(9);
        item->pl_russia_id = row.get_i(10);
        item->pl_russia_mob_id = row.get_i(11);
        item->pl_intern_id = row.get_i(12);
        item->pl_sng_id = row.get_i(13);
    }

    inline long long int key(UsageObj *item) {
        return item->phone_num;
    }


public:
    UsageObj * find(const char * phone_num) {
        return (UsageObj *) _find(atoll(phone_num));
    }

    UsageObj * find(const long long int phone_num) {
        return (UsageObj *) _find(phone_num);
    }
};
