#include "PhoneList.h"

#include "../App.h"

size_t PhoneList::item_size() { return 0; }
string PhoneList::sql(BDb * db) { return ""; }
void PhoneList::parse_item(BDbResult &row, void * obj) { *(int*)obj = row.size(); }


Phone & PhoneList::get(const long long int phone_num) {
    return list[phone_num];
}
void PhoneList::set(long long int phone_num,  Phone & new_value) {
    list[phone_num] = new_value;
}
void PhoneList::clear() {
    list.clear();
}


void PhoneList::load(BDb * db, time_t dt) {

    t.start();

    this->dt = dt;
    list.clear();


    string s_time = string_date(dt);
    string s_region = lexical_cast<string>(app.conf.geo_region);

    BDbResult res = db->query("      select  u.phone_num, u.id, u.client_id, u.region, " \
                              "           t1.freemin*60* case t1.freemin_for_number when true then 1 else u.no_of_lines end, " \
                              "           t1.paid_redirect, " \
                              "           t1.pricelist_id, t2.pricelist_id, t3.pricelist_id, t4.pricelist_id, t5.pricelist_id " \
                              "       from billing.usage u " \
                              "       left join ( " \
                              "               select l.usage_id, l.tarif_id_local, l.tarif_id_local_mob, l.tarif_id_russia, l.tarif_id_intern, l.tarif_id_sng from ( " \
                              "                       select usage_id, max(id) as max_id from billing.tarif_change_log " \
                              "                       where date_activation<='"+s_time+"' " \
                              "                       group by usage_id ) as lm " \
                              "                       inner join billing.tarif_change_log l on l.id = lm.max_id " \
                              "               )as lt on u.id=lt.usage_id " \
                              "       left join billing.tarif t1 on t1.id = lt.tarif_id_local " \
                              "       left join billing.tarif t2 on t2.id = lt.tarif_id_local_mob " \
                              "       left join billing.tarif t3 on t3.id = lt.tarif_id_russia " \
                              "       left join billing.tarif t4 on t4.id = lt.tarif_id_intern " \
                              "       left join billing.tarif t5 on t5.id = lt.tarif_id_sng " \
                              "       where u.region='"+s_region+"' and u.actual_from <= '"+s_time+"' and u.actual_to >= '"+s_time+"' " \
                              "       order by phone_num::BIGINT asc, u.id asc ");

    loadtime = time(NULL);
    last_use = loadtime;
    while(res.next()){
        Phone &phone = list[res.get_ll(0)];
        phone.phone_num = res.get_ll(0);
        phone.usage_id = res.get_i(1);
        phone.client_id = res.get_i(2);
        phone.region = res.get_i(3);
        phone.freemin = res.get_i(4);
        phone.paid_redirect = res.get_b(5);
        phone.pl_local_id = res.get_i(6);
        phone.pl_local_mob_id = res.get_i(7);
        phone.pl_russia_id = res.get_i(8);
        phone.pl_intern_id = res.get_i(9);
        phone.pl_sng_id = res.get_i(10);
    }

    t.stop();

    loaded = true;
}
