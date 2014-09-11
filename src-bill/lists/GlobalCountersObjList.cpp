#include "GlobalCountersObjList.h"
#include "../classes/AppBill.h"

size_t GlobalCountersObjList::item_size() {
    return sizeof (GlobalCountersObj);
}

string GlobalCountersObjList::sql(BDb * db) {
    string instance_id = app().conf.str_instance_id;
    string sDay = string_date(get_tday());
    string sMonth = string_date(get_tmonth());
    return "   select client_id, sum(amount_sum), " \
           "          sum(case when amount_day = '" + sDay + "' then amount_day_sum else 0 end), " \
           "          sum(case when amount_month = '" + sMonth + "' then amount_month_sum else 0 end) " \
           "   from billing.clients_counters " \
           "     where region_id<>" + instance_id + " and (amount_sum<>0 or amount_day_sum<>0 or amount_month_sum<>0) " \
           "     group by client_id " \
           "     order by client_id asc ";
}

inline void GlobalCountersObjList::parse_item(BDbResult &row, void * obj) {
    pGlobalCountersObj item = (pGlobalCountersObj) obj;
    item->client_id = row.get_i(0);
    item->sum = row.get_i(1);
    item->sum_day = row.get_i(2);
    item->sum_month = row.get_i(3);
}

inline int GlobalCountersObjList::key(const void *obj) {
    return ( (pGlobalCountersObj) obj)->client_id;
}

pGlobalCountersObj GlobalCountersObjList::find(const int client_id) {
    return (pGlobalCountersObj) _find(client_id);
}
