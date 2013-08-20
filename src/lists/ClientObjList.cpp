#include "ClientObjList.h"

size_t ClientObjList::item_size(){	return sizeof(ClientObj);	}
string ClientObjList::sql(BDb * db){
    return		"   select id, voip_limit_month, voip_limit_day, credit, balance, amount_date, last_payed_month, voip_disabled " \
                "   from billing.clients " \
                "   order by id asc ";
}
void ClientObjList::parse_item(BDbResult &row, void * obj){
    pClientObj item = (pClientObj)obj;
    item->id = row.get_i(0);
    item->limit_m = row.get_i(1);
    item->limit_d = row.get_i(2);
    item->credit = row.get_i(3);
    item->balance = row.get_d(4);
    item->amount_date = parseDateTime(row.get(5));
    item->last_payed_month = parseDate(row.get(6));
    item->disabled = row.get_b(7);
}
int ClientObjList::key(void *obj){
    return( (pClientObj)obj)->id;
}

pClientObj ClientObjList::find(int client_id) {
    return (pClientObj)_find(client_id);
}


