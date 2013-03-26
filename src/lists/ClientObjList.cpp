#include "ClientObjList.h"

size_t ClientObjList::item_size(){	return sizeof(ClientObj);	}
string ClientObjList::sql(BDb * db){
    return		"   select id, voip_limit_month, voip_limit_day, credit, balance, amount_date, voip_disabled " \
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
    item->disabled = row.get_b(6);


    struct tm ttt;
    ttt.tm_isdst = 0; ttt.tm_wday = 0; ttt.tm_yday = 0;
    sscanf(row.get(5), "%d-%d-%d %d:%d:%d", &ttt.tm_year, &ttt.tm_mon, &ttt.tm_mday, &ttt.tm_hour, &ttt.tm_min, &ttt.tm_sec);
    ttt.tm_year -= 1900; ttt.tm_mon -= 1;
    item->amount_date = mktime(&ttt);
}
int ClientObjList::key(void *obj){
    return( (pClientObj)obj)->id;
}

pClientObj ClientObjList::find(int client_id) {
    return (pClientObj)_find(client_id);
}


