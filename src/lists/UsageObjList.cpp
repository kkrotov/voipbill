#include "UsageObjList.h"
#include <stdlib.h>
#include "../App.h"

size_t UsageObjList::item_size(){	return sizeof(UsageObj);	}

string UsageObjList::sql(BDb * db){
    string time = string_date(dt);
    string region = lexical_cast<string>(app.conf.geo_region);
    return	"      select  u.phone_num, u.id, u.client_id, u.region, " \
            "           t1.freemin*60* case t1.freemin_for_number when true then 1 else u.no_of_lines end, " \
            "           t1.paid_redirect, " \
            "           t1.pricelist_id, t2.pricelist_id, t3.pricelist_id, t4.pricelist_id, t5.pricelist_id " \
            "       from billing.usage u " \
            "       left join ( " \
            "               select l.usage_id, l.tarif_id_local, l.tarif_id_local_mob, l.tarif_id_russia, l.tarif_id_intern, l.tarif_id_sng from ( " \
            "                       select usage_id, max(id) as max_id from billing.tarif_change_log " \
            "                       where date_activation<='"+time+"' " \
            "                       group by usage_id ) as lm " \
            "                       inner join billing.tarif_change_log l on l.id = lm.max_id " \
            "               )as lt on u.id=lt.usage_id " \
            "       left join billing.tarif t1 on t1.id = lt.tarif_id_local " \
            "       left join billing.tarif t2 on t2.id = lt.tarif_id_local_mob " \
            "       left join billing.tarif t3 on t3.id = lt.tarif_id_russia " \
            "       left join billing.tarif t4 on t4.id = lt.tarif_id_intern " \
            "       left join billing.tarif t5 on t5.id = lt.tarif_id_sng " \
            "       where u.region='"+region+"' and u.actual_from <= '"+time+"' and u.actual_to >= '"+time+"' " \
            "           and t1.pricelist_id is not null and t2.pricelist_id is not null and t3.pricelist_id is not null and t4.pricelist_id is not null and t5.pricelist_id is not null " \
            "       order by phone_num::BIGINT asc, u.id asc ";
}

void UsageObjList::parse_item(BDbResult &row, void * obj){
    pUsageObj item = (pUsageObj)obj;
    item->phone_num = row.get_ll(0);
    item->id = row.get_i(1);
    item->client_id = row.get_i(2);
    item->region = row.get_i(3);
    item->freemin = row.get_i(4);
    item->paid_redirect = row.get_b(5);
    item->pl_local_id = row.get_i(6);
    item->pl_local_mob_id = row.get_i(7);
    item->pl_russia_id = row.get_i(8);
    item->pl_intern_id = row.get_i(9);
    item->pl_sng_id = row.get_i(10);
}

long long int UsageObjList::key(void *obj){
    return ((pUsageObj)obj)->phone_num;
}

pUsageObj UsageObjList::find(const char * phone) {
    return (pUsageObj)_find(atoll(phone));
}

pUsageObj UsageObjList::find(const long long int phone) {
    return (pUsageObj)_find(phone);
}

